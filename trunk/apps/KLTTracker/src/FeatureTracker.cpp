#include "FeatureTracker.h"

#include <fstream>

#include "MiscUtils.h"

using std::stringstream;
using std::ifstream;
using std::ios;

const int OUTLIER = -999;
const unsigned int BUFFER_SIZE = 1024;
AnyImage<unsigned char> rgbBuffer(BUFFER_SIZE, BUFFER_SIZE, 3, 255);

using std::string;
using std::vector;
using std::map;
using std::cout;
using std::endl;

FeatureTracker::FeatureTracker()
: nFeatures(0), tracking_context(0), feature_list(0),
 feature_table(0), textures(1)
{}

FeatureTracker::FeatureTracker(const FeatureTracker &other)
{
    if(this != &other)
        assert(false);
}

FeatureTracker& FeatureTracker::operator=(const FeatureTracker &other)
{
    if(this != &other)
        assert(false);
    return *this;
}

void FeatureTracker::init(int nFeatures, int nFrames)
{
    assert(tracking_context == 0);

    tracking_context = KLTCreateTrackingContext();
    feature_list = KLTCreateFeatureList(nFeatures);
    feature_table = KLTCreateFeatureTable(nFrames, nFeatures);

    tracking_context->sequentialMode = TRUE;
    tracking_context->writeInternalImages = FALSE;
    tracking_context->affineConsistencyCheck = -1;

    tracking_context->mindist = 10;
    tracking_context->window_width  = 7;
    tracking_context->window_height = 7;
//     tracking_context->nPyramidLevels = 2;
//     tracking_context->subsampling = 2;
//    tracking_context->max_residue = 20;
    KLTSetVerbosity(0);
}

void FeatureTracker::init(int nFeatures, int nFrames, const std::string &param_filename)
{
    assert(tracking_context == 0);
    assert(nFeatures > 0);
    assert(nFrames > 0);

    // init tracking context
    tracking_context = KLTCreateTrackingContext();
    feature_list = KLTCreateFeatureList(nFeatures);
    feature_table = KLTCreateFeatureTable(nFrames, nFeatures);
    tracking_context->sequentialMode = TRUE;
    tracking_context->writeInternalImages = FALSE;

    // open file
    ifstream in(param_filename.c_str(), ios::in);
    if (in.fail())
        cout << "Failed to load open parameter file: " << param_filename << endl << endl;
    else
    {
        // read all parameters
        unsigned int line_count = 0;
        string line, keyword, value;
        while (!in.eof())
        {
            getline(in, line);
            line_count++;

            stringstream ss(line);
            ss >> keyword;
            keyword = string(keyword);

            if(keyword == "mindist")
                ss >> tracking_context->mindist;
            else if(keyword == "min_eigenvalue")
                ss >> tracking_context->min_eigenvalue;
            else if(keyword == "window_width")
                ss >> tracking_context->window_width;
            else if(keyword == "window_height")
                ss >> tracking_context->window_height;
            else if(keyword == "nPyramidLevels")
                ss >> tracking_context->nPyramidLevels;
            else if(keyword == "subsampling")
                ss >> tracking_context->subsampling;
            else if(keyword == "max_residue")
                ss >> tracking_context->max_residue;
            else if(keyword == "affineConsistencyCheck")
                ss >> tracking_context->affineConsistencyCheck;
            else
                cout << "!!! Line " << line_count << " ignored: [" << line << "] with keyword = [" << keyword << "]" << endl;
        }
    }
    in.close();

    cout << "Tracking Parameters:" << endl;
    cout << "mindist = " << tracking_context->mindist << endl;
    cout << "min_eigenvalue = " << tracking_context->min_eigenvalue << endl;
    cout << "window_width = " << tracking_context->window_width << endl;
    cout << "window_height = " << tracking_context->window_height << endl;
    cout << "nPyramidLevels = " << tracking_context->nPyramidLevels << endl;
    cout << "subsampling = " << tracking_context->subsampling << endl;
    cout << "max_residue = " << tracking_context->max_residue << endl;
    cout << "affineConsistencyCheck = " << tracking_context->affineConsistencyCheck << endl;

    KLTSetVerbosity(0);
}

void FeatureTracker::initTracker(int num_features, int first_track, int last_track, const std::string &param_filename)
{
    nFeatures = num_features;
    first_track_frame = first_track;
    last_track_frame = last_track;
    assert(first_track_frame > 0);
    assert(first_track_frame <= last_track_frame);

    glGenTextures(1, &textures[0]);
    init(nFeatures, last_track_frame-first_track_frame+1, param_filename);}

vector<unsigned int> FeatureTracker::countFeatures(int frame, bool only_tracked) const
{
    assert((frame+first_track_frame) <= last_track_frame);

    vector<unsigned int> features;
    if(only_tracked)
    {
        for(int i = 0; i < nFeatures; i++)
            if(feature_table->feature[i][frame]->val == 0)
                features.push_back(i);
    }
    else
    {
        for(int i = 0; i < nFeatures; i++)
            if(feature_table->feature[i][frame]->val >= 0)
                features.push_back(i);
    }

    return features;
}

Array2D<double> FeatureTracker::getAllFeatures(int frame) const
{
    assert((frame+first_track_frame) <= last_track_frame);

    Array2D<double> feature_pos(nFeatures, 3);
    for(int i = 0; i < nFeatures; i++)
    {
        feature_pos(i, 0) = feature_table->feature[i][frame]->x;
        feature_pos(i, 1) = feature_table->feature[i][frame]->y;
        feature_pos(i, 2) = feature_table->feature[i][frame]->val;
    }

    return feature_pos;
}

void FeatureTracker::writeFeatures(const string &filename) const
{
    assert(tracking_context != 0);

    KLTWriteFeatureTable(feature_table, filename.c_str(), "%5.1f");
}

bool FeatureTracker::doTracking(const AnyImage<unsigned char> &new_frame, int framenumber)
{
    if((framenumber < first_track_frame) || (framenumber > last_track_frame))
        return false;

    temp_rgbimage = new_frame;
    temp_grayimage = rgb2gray(new_frame);
    if(framenumber == first_track_frame)
    {
        selectGoodFeatures(temp_grayimage, 0);
        first_grayimage = temp_grayimage;
    }
    else
        trackFeatures(first_grayimage, temp_grayimage, framenumber-first_track_frame, true);

    return true;
}

void FeatureTracker::drawFeatures(int framenumber, int windowHeight)
{
    if((framenumber < first_track_frame) || (framenumber > last_track_frame))
        return;

    int frame = framenumber - first_track_frame;

    glDisable(GL_TEXTURE_2D);
    glPointSize(3.0);
    glBegin(GL_POINTS);
    for(int i = 0; i < nFeatures; i++)
    {
        if(FeatureTracker::feature_table->feature[i][frame]->val < 0)
            glColor3f(1.0, 0.0, 0.0);
        else if(FeatureTracker::feature_table->feature[i][frame]->val == 0)
            glColor3f(0.0, 1.0, 0.0);
        else
            glColor3f(0.0, 0.0, 1.0);

        glVertex2f(FeatureTracker::feature_table->feature[i][frame]->x, windowHeight-FeatureTracker::feature_table->feature[i][frame]->y);
    }
    glEnd();

    glLineWidth(2.0);
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINES);
    for(int i = 0; i < nFeatures; i++)
    {
        const int temp_val = FeatureTracker::feature_table->feature[i][frame]->val;

        if(temp_val == 0)
            glColor3f(0.0, 1.0, 0.0);
        else if(temp_val == OUTLIER)
            glColor3f(0.0, 0.0, 0.0);

        if((temp_val == 0) || (temp_val == OUTLIER))
        {
            glVertex2f(FeatureTracker::feature_table->feature[i][frame-1]->x, windowHeight-FeatureTracker::feature_table->feature[i][frame-1]->y);
            glVertex2f(FeatureTracker::feature_table->feature[i][frame]->x, windowHeight-FeatureTracker::feature_table->feature[i][frame]->y);
        }
    }
    glEnd();
}

void FeatureTracker::drawFrame(const AnyImage<unsigned char> &image_to_draw, int windowWidth, int windowHeight)
{
    AnyImage<unsigned char> temp_image = image_to_draw;

    glEnable(GL_TEXTURE_2D);
    glColor3f(0.0, 0.0, 0.0);
    rgbBuffer.copyFromImage(temp_image);

    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BUFFER_SIZE, BUFFER_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*) rgbBuffer.data().vals());
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    double w = image_to_draw.width();
    double h = image_to_draw.height();

    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, h/BUFFER_SIZE);
        glVertex3f(0.0f, 0.0f, 0.0f);

        glTexCoord2f(w/BUFFER_SIZE, h/BUFFER_SIZE);
        glVertex3f(windowWidth, 0.0f, 0.0f);

        glTexCoord2f(w/BUFFER_SIZE, 0.0f);
        glVertex3f(windowWidth, windowHeight, 0.0f);

        glTexCoord2f(0.0f,0.0f);
        glVertex3f(0.0f, windowHeight, 0.0f);
    glEnd();
}

void FeatureTracker::selectGoodFeatures(AnyImage<unsigned char> &image, int frame)
{
    assert(tracking_context != 0);

    KLTSelectGoodFeatures(tracking_context, image.data().vals(), image.width(), image.height(), feature_list);
    KLTStoreFeatureList(feature_list, feature_table, frame);
}

void FeatureTracker::trackFeatures(AnyImage<unsigned char> &from_image, AnyImage<unsigned char> &to_image, int frame, bool replace)
{
    assert(tracking_context != 0);
    assert(frame > 0);  // can only start tracking from the second frame (zero indexed)
    assert(frame < feature_table->nFrames);

    for(int i = 0; i < feature_list->nFeatures; i++)
        feature_list->feature[i]->val = feature_table->feature[i][frame-1]->val;

    KLTTrackFeatures(tracking_context, from_image.data().vals(), to_image.data().vals(), to_image.width(), to_image.height(), feature_list);
    if(replace) KLTReplaceLostFeatures(tracking_context, to_image.data().vals(), to_image.width(), to_image.height(), feature_list);
    KLTStoreFeatureList(feature_list, feature_table, frame);
}

unsigned int countDigits(int number)
{
    unsigned int digit = 1;
    while(number > 0)
    {
        number /= 10;
        if(number > 0) digit++;
    }

    return digit;
}

string getFrameString(int current_frame, int total_frames)
{
    string temp_string;
    int current_digits = countDigits(current_frame);
    int total_digits = countDigits(total_frames);

    for(int i = current_digits; i < total_digits; i++)
        temp_string += "0";
    temp_string += stream_cast<string, int>(current_frame);

    return temp_string;
}
