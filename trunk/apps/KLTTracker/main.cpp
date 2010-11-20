#ifdef WIN32
#include <GL/freeglut.h>
#else
#include <GL/glut.h>
#endif

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <cassert>

#include "MiscUtils.h"
#include "GLUtils.h"
#include "FeatureTracker.h"
#include "LinearAlgebra.h"
#include "VideoStream.h"

using std::vector;
using std::string;
using std::cout;
using std::endl;



// variables for controlling the app
bool fix_pointer = false;
bool freeze_frame = true;
bool force_track_next_frame = false;
// window size
int window_width = 320;
int window_height = 240;
int mainID = 0;
// frame rate control
double desiredFPS = 60.0;
double elapsedTime = 0.0;
double lastTime = -1.0;
double fps = 0.0;

VideoStream *stream;
FeatureTracker feature_tracker;
unsigned int next_frame_to_track = 0;



bool decodeNextFrame(unsigned int num_iters)
{
    bool got_frame = false;
    for(unsigned int i = 0; i < num_iters; i++)
        if(stream->decode_next_frame(PIX_FMT_RGB24))   // keep decoding until find next frame
        {
            got_frame = true;
            break;
        }

    return got_frame;
}

void saveFeatures(const string &filename)
{
    int num_of_tracked_frames = next_frame_to_track-feature_tracker.first_track_frame;
    Array2D<double> features(feature_tracker.nFeatures+1, num_of_tracked_frames*3);

    for(int i = 0; i < num_of_tracked_frames; i++)
    {
        features(0, i*3+0) = i+feature_tracker.first_track_frame;
        features(0, i*3+1) = i+feature_tracker.first_track_frame;
        features(0, i*3+2) = i+feature_tracker.first_track_frame;

        setsub(features, 1, feature_tracker.nFeatures, i*3+0, i*3+2, feature_tracker.getAllFeatures(i));
    }

    std::ofstream out(filename.c_str(), std::ios::out);
    out << features << endl;
    out.close();
}

void idle()
{
}

void keyboard(unsigned char key, int x, int y)
{
    int tempID = glutGetWindow();
    switch (key)
    {
        case 27:        cout << "about to exit " << endl;
                        exit(0);
        case (int)('f'):fix_pointer = !fix_pointer;
                        tempID = glutGetWindow();
                        if (fix_pointer)
                        {
                            glutSetWindow(mainID);
                            glutSetCursor(GLUT_CURSOR_NONE);
                            glutSetWindow(tempID);
                        }
                        else
                        {
                            glutSetWindow(mainID);
                            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
                            glutSetWindow(tempID);
                        }
                        break;
        case (int)('n'):freeze_frame = !freeze_frame;
                        break;
        case (int)('m'):force_track_next_frame = true;
                        break;
        case (int)('c'):takeScreenShot("screenshot.ppm");
                        break;
        case (int)('s'):saveFeatures("features.txt");
                        feature_tracker.writeFeatures("features.log");
        default:        break;
    }
}

void keyboardUp(unsigned char key, int x, int y)
{
    switch(key)
    {
        case (int)('f'):
        case (int)('b'):
        case (int)('n'):
        case (int)('m'):
        case (int)('c'):
        case (int)('s'):
        default:        break;
    }
}

void motion(int x, int y)
{
}

void mouseMotion(int x, int y)
{
}

void mouse(int button, int state, int x, int y)
{
    cout << "Clicked at [" << x << " " << y << "]" << endl;
}

void resize(int width, int height)
{
    window_width = static_cast<int>(width);
    window_height = static_cast<int>(height);

    glViewport(0, 0, window_width, window_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, window_width, 0.0, window_height, -1.0, 1.0 );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void draw_opengl()
{
    if (lastTime < 0.0)
        lastTime = getTime();

    double currentTime = getTime();
    double deltaTime = currentTime - lastTime;
    elapsedTime += deltaTime;
    lastTime = currentTime;

//     glEnable(GL_TEXTURE_2D);                                                // Enable Texture Mapping ( NEW )
    glShadeModel(GL_SMOOTH);                                                // Enable Smooth Shading
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);                                   // Black Background
    glClearDepth(1.0f);                                                     // Depth Buffer Setup
    glDisable(GL_DEPTH_TEST);                                               // Enables Depth Testing
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glDisable( GL_LIGHTING );

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if(!freeze_frame || force_track_next_frame)
    {
        if(decodeNextFrame(20))
        {
            Array2D<unsigned char> raw_image(3, stream->frame_width()*stream->frame_height(), stream->data());
            AnyImage<unsigned char> new_frame(stream->frame_width(), stream->frame_height(), raw_image);

            if(next_frame_to_track == stream->frame_count())
            {
                if(feature_tracker.doTracking(new_frame, stream->frame_count()))
                    next_frame_to_track++;
                // else not part of the sequence to be tracked
            }
            else
                feature_tracker.temp_rgbimage = new_frame;
        }

        force_track_next_frame = false;
    }

    feature_tracker.drawFrame(feature_tracker.temp_rgbimage, window_width, window_height);
    feature_tracker.drawFeatures(stream->frame_count(), window_height);

    glutPostRedisplay();
    glutSwapBuffers();

    char title[100];
    sprintf(title, "OpenGL Template: Frame %d (%d) @ %f fps", stream->frame_count(), next_frame_to_track, fps);
    glutSetWindowTitle(title);

    // simple method for controlling speed of animation
    if (elapsedTime > (1.0 / desiredFPS))
    {
        fps = 1 / elapsedTime;
        elapsedTime = 0.0;
    }
}

void setupWindows()
{
    glutInitWindowSize(window_width, window_height);
    glutInitWindowPosition(0, 0);
    mainID = glutCreateWindow("OpenGL Template");
    glutDisplayFunc(draw_opengl);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    glutPassiveMotionFunc(motion);
    glutReshapeFunc(resize);
    glutIdleFunc(idle);
//     glutSetCursor(GLUT_CURSOR_NONE);
    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

    glutSetWindow(mainID);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if(argc < 6)
    {
        cout << "USAGE:" << endl;
        cout << "AppearanceTracker <video_filename> <param_filename> <number_of_features> <first_track_frame> <last_track_frame>" << endl;
        exit(1);
    }

    string video_filename = string(argv[1]);
    string param_filename = string(argv[2]);
    int nFeatures = atoi(argv[3]);
    int first_track_frame = atoi(argv[4]);
    int last_track_frame = atoi(argv[5]);
    stream = new VideoStream(video_filename);
    feature_tracker.initTracker(nFeatures, first_track_frame, last_track_frame, param_filename);

    window_width = stream->frame_width();
    window_height = stream->frame_height();
    next_frame_to_track = first_track_frame;

    setupWindows();
    glutMainLoop();
}
