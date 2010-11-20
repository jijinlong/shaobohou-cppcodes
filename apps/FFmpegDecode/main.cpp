#include <iostream>
#include <sstream>
#include <cmath>
#include <vector>
#include <string>

#include <GL/glut.h>
#include <cassert>

#include "VideoStream.h"

using std::ostringstream;
using std::vector;
using std::string;
using std::cout;
using std::endl;

struct StreamInfo
{
    int win_id;
    VideoStream *stream;
    int desired_frame_number;

    unsigned int win_width;
    unsigned int win_height;
};

// texture variables
unsigned int buffer_size = 512;
unsigned char *rgb_buffer = 0;
GLuint *textures;

// frame and control
bool freeze_frame = false;
vector<StreamInfo> stream_infos;

// streams variable
vector<string> filenames;



void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27:
            cout << "about to exit " << endl;
            exit(0);
            break;
//         case (int)('b'):    if (desired_frame_number >= 0)
//                             desired_frame_number--;
//             break;
        case (int)('n'):    freeze_frame = !freeze_frame;
            break;
        case (int)('m'):    for(unsigned int i = 0; i < stream_infos.size(); i++)
                                stream_infos[i].desired_frame_number++;
            break;
        default:
            break;
    }
}

// void keyboardUp(unsigned char key, int x, int y)
// {
// }
//
// void motion(int x, int y)
// {
// }
//
// void mouseMotion(int x, int y)
// {
// }
//
// void mouse(int button, int state, int x, int y)
// {
// }
//
// void idle()
// {
// }

void resize(int width, int height)
{
    unsigned int current_id = stream_infos.size();
    for(unsigned int i = 0; i < stream_infos.size(); i++)
        if(stream_infos[i].win_id == glutGetWindow())
            current_id = i;
    assert(current_id < stream_infos.size());

    double good_ratio = static_cast<double>(stream_infos[current_id].win_width) / static_cast<double>(stream_infos[current_id].win_height);
    double true_ratio = static_cast<double>(width) / static_cast<double>(height);

    if (true_ratio > good_ratio)
    {
        stream_infos[current_id].win_width = static_cast<int>(round(height * good_ratio));
        stream_infos[current_id].win_height = static_cast<int>(height);
    }
    else
    {
        stream_infos[current_id].win_width = static_cast<int>(width);
        stream_infos[current_id].win_height = static_cast<int>((width / good_ratio));
    }

    glViewport(0, 0, stream_infos[current_id].win_width, stream_infos[current_id].win_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, stream_infos[current_id].win_width, 0.0, stream_infos[current_id].win_height, -1.0, 1.0 );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void draw_opengl()
{
    unsigned int current_id = stream_infos.size();
    for(unsigned int i = 0; i < stream_infos.size(); i++)
        if(stream_infos[i].win_id == glutGetWindow())
            current_id = i;
    assert(current_id < stream_infos.size());

    bool got_frame = false;
    unsigned int num_tries = 20;
    if((!freeze_frame) || (stream_infos[current_id].desired_frame_number > stream_infos[current_id].stream->frame_count()))
        for(unsigned int i = 0; i < num_tries; i++)
            if(stream_infos[current_id].stream->decode_next_frame(PIX_FMT_RGB24))   // keep decoding until find next frame
            {
                got_frame = true;
                break;
            }
    if(!got_frame) freeze_frame = true;

    unsigned int buffer_width = stream_infos[current_id].stream->frame_width();
    unsigned int buffer_height = stream_infos[current_id].stream->frame_height();
    unsigned char *real_buffer = stream_infos[current_id].stream->data();

#ifdef WIN32
    stream_infos[current_id].stream->fillBuffer(rgb_buffer, buffer_size, buffer_size);
    buffer_width = buffer_size;
    buffer_height = buffer_size;
    real_buffer = rgb_buffer;
#endif

    glBindTexture(GL_TEXTURE_2D, textures[current_id]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, buffer_width, buffer_height, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*) real_buffer);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glEnable(GL_TEXTURE_2D);                                                // Enable Texture Mapping ( NEW )
    glShadeModel(GL_SMOOTH);                                                // Enable Smooth Shading
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);                                   // Black Background
    glClearDepth(1.0f);                                                     // Depth Buffer Setup
    glDisable(GL_DEPTH_TEST);                                               // Enables Depth Testing
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glDisable( GL_LIGHTING );

    double w = stream_infos[current_id].stream->frame_width();
    double h = stream_infos[current_id].stream->frame_height();

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, h/buffer_height);
    glVertex2f(0.0, 0.0);

    glTexCoord2f(w/buffer_width, h/buffer_height);
	glVertex2f( stream_infos[current_id].win_width, 0.0);

    glTexCoord2f(w/buffer_width, 0.0);
	glVertex2f( stream_infos[current_id].win_width, stream_infos[current_id].win_height);

    glTexCoord2f(0.0, 0.0);
	glVertex2f(0.0, stream_infos[current_id].win_height);
    glEnd();

    char title[100];
    sprintf(title, "Motion: Frame %d (%d)", stream_infos[current_id].desired_frame_number, stream_infos[current_id].stream->frame_count());
    glutSetWindowTitle(title);
    if (!freeze_frame)
        stream_infos[current_id].desired_frame_number++;

    glutPostRedisplay();
    glutSwapBuffers( );
}

void setupWindows(const vector<string> &filenames)
{
    textures = new GLuint[filenames.size()];
    glGenTextures(filenames.size(), textures);

    stream_infos = vector<StreamInfo>(filenames.size());
    for(unsigned int i = 0; i < filenames.size(); i++)
    {
        stream_infos[i].desired_frame_number = 0;
        stream_infos[i].stream = new VideoStream(filenames[i]);
        stream_infos[i].win_width = stream_infos[i].stream->frame_width();
        stream_infos[i].win_height = stream_infos[i].stream->frame_height();

        while(stream_infos[i].win_width > buffer_size) buffer_size *= 2;
        while(stream_infos[i].win_height > buffer_size) buffer_size *= 2;

        ostringstream ss;
        ss << filenames[i];

        glutInitWindowSize(stream_infos[i].win_width, stream_infos[i].win_height);
        glutInitWindowPosition(0+100*i, 0);
        stream_infos[i].win_id = glutCreateWindow(ss.str().c_str());
        glutDisplayFunc(draw_opengl);
        glutKeyboardFunc(keyboard);
        glutReshapeFunc(resize);
        glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
    }

    rgb_buffer = new unsigned char[buffer_size*buffer_size*3];

    glutSetWindow(stream_infos[0].win_id);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Usage:" << endl;
        cout << "ffmepg_decode [video_files] ..." << endl;
        exit(1);
    }

    for(int i = 1; i < argc; i++)
        filenames.push_back(argv[i]);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    setupWindows(filenames);

    glutMainLoop();
}
