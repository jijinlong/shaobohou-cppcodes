#ifndef FEATURE_TRACKER
#define FEATURE_TRACKER

#ifdef WIN32
#include <GL/freeglut.h>
#else
#include <GL/glut.h>
#endif

#include <string>
#include <vector>
#include <map>

#include "klt.h"
#include "ImageFormat.h"

// in frames 0, the val is the min eigenvalue
// in other frames 0 means it is tracked, -? means it is lost

class FeatureTracker
{
    public:
        AnyImage<unsigned char> temp_rgbimage, temp_grayimage, first_grayimage;
        int first_track_frame, last_track_frame;
        int nFeatures;

        FeatureTracker();
        FeatureTracker(const FeatureTracker &other);
        FeatureTracker& operator=(const FeatureTracker &other);

        void initTracker(int num_features, int first_track_frame, int last_track_frame, const std::string &param_filename);
        bool doTracking(const AnyImage<unsigned char> &new_frame, int framenumber);

        std::vector<unsigned int> countFeatures(int frame, bool only_tracked) const;
        Array2D<double> getAllFeatures(int frame) const;
        void writeFeatures(const std::string &filename) const;

        void drawFeatures(int frame, int windowHeight);
        void drawFrame(const AnyImage<unsigned char> &image_to_draw, int window_width, int window_height);

    private:
        KLT_TrackingContext tracking_context;
        KLT_FeatureList feature_list;
        KLT_FeatureTable feature_table;

        std::vector<GLuint> textures;

        void init(int num_features, int num_frames);
        void init(int num_features, int num_frames, const std::string &param_filename);

        // no const on image argument because of the c interface doesnn't allow it
        void selectGoodFeatures(AnyImage<unsigned char> &image, int frame);
        void trackFeatures(AnyImage<unsigned char> &from_image, AnyImage<unsigned char> &to_image, int frame, bool replace);
};

unsigned int countDigits(int number);
std::string getFrameString(int current_frame, int total_frames);

#endif
