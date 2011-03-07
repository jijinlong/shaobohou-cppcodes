#if defined _WIN64 || defined _WIN32
#include <windows.h>
#endif

#include "Visualisation.h"
#include "BiovisionReader.h"
#include "BiovisionDisplay.h"
#include "QuakeCamera.h"
#include "MiscUtils.h"
#include "special.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <GL/gl.h>
#include <GL/glut.h>

using std::vector;
using std::cout;
using std::endl;
using std::string;
using std::ios;
using std::ofstream;


// BVH stuff
std::vector<BiovisionModel> models;
std::vector<BiovisionReader> readers;
std::vector<BiovisionAnimation> animations;

// control variables
QuakeCamera camera;
int framenumber = 0;
bool fixPointer = true;
bool freezeFrame = true;
// window stuff
int mainID = -1;
int windowWidth = 640;
int windowHeight = 480;
// frame rate control
double desiredFPS = 60.0;
double elapsedTime = 0.0;
double lastTime = -1.0;
double fps = 0.0;


void idle()
{
}

void keyboard(unsigned char key, int x, int y)
{
    int tempID = glutGetWindow();
    switch(key)
    {
        case 27:        cout << "about to exit " << endl;
                        exit(0);
        case (int)('f'):fixPointer = !fixPointer;
    			        tempID = glutGetWindow();
    			        if(fixPointer)
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
        case (int)('b'):if(framenumber >= 0)
                            framenumber--;
                        else
                            framenumber = animations.front().nframes();
  			            break;
        case (int)('n'):freezeFrame = !freezeFrame;
  			            break;
        case (int)('m'):framenumber++;
  			            break;
        default:		camera.move.set(key, true);
                        break;
    }
}

void keyboardUp(unsigned char key, int x, int y)
{
    switch(key)
    {
        case (int)('f'):
        case (int)('b'):
        case (int)('n'):
        case (int)('m'):break;
        default:        camera.move.set(key, false);
                        break;
    }
}

void motion(int x, int y)
{
    if(fixPointer)
    {
        camera.modOrientation((double)(x - (windowWidth / 2.0))/ (double)windowWidth, (double)(windowHeight - y - (windowHeight / 2.0))/ (double)windowHeight);

        if(((x - windowWidth / 2) != 0) || ((y - windowHeight / 2) != 0))
        glutWarpPointer(windowWidth / 2, windowHeight / 2);
    }
}

void mouseMotion(int x, int y)
{
}

void mouse(int button, int state, int x, int y)
{
}

void resize(int width, int height)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    windowWidth = width;
    windowHeight = height;
    double aspect = (double)width / (double)height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, aspect, 0.1, 1000000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void setCamera(double realTime)
{
    Vector3D velocity = camera.update(realTime);
    camera.setPosition(camera.getPosition() + velocity);
    camera.setPerspective();
}

void draw_opengl()
{
    if(lastTime < 0.0)
        lastTime = getTime();

    double currentTime = getTime();
    double deltaTime = currentTime - lastTime;
    elapsedTime += deltaTime;
    lastTime = currentTime;


    glShadeModel(GL_SMOOTH);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);

    //glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);

    glEnable(GL_NORMALIZE);

    GLfloat lightPos[] = {0.0, 0.0, 0.0, 1.0},
            white[] = {1.0, 1.0, 1.0, 1.0};

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();


    setCamera(deltaTime);	//everything must be drawn after setting camera

    glPushMatrix();

        glPushMatrix();

            glColor3f(0.0, 1.0, 0.0);
            glBegin(GL_LINES);
                glNormal3f(0.0, 1.0, 0.0);
                for(int i = -500; i < 501; i+=100)
                {
                    glVertex3f(i, 0.0, -500.0);
                    glVertex3f(i, 0.0, 500.0);
                    glVertex3f(-500.0, 0.0, i);
                    glVertex3f(500.0, 0.0, i);
                }

                glColor3f(1.0, 0.0, 0.0);
                glVertex3f(0.0, 0.0, 0.0);
                glVertex3f(1000.0, 0.0, 0.0);
                glColor3f(0.0, 1.0, 0.0);
                glVertex3f(0.0, 0.0, 0.0);
                glVertex3f(0.0, 1000.0, 0.0);
                glColor3f(0.0, 0.0, 1.0);
                glVertex3f(0.0, 0.0, 0.0);
                glVertex3f(0.0, 0.0, 1000.0);
            glEnd();

            for(unsigned int i = 0; i < models.size(); i++)
                setPose(models[i], animations[i].frames(framenumber % animations[i].nframes()));
//            std::cout << framenumber << "  " << computeAlignment(Quaternion(1.0, Vector3D()), model.root().local_orientation(), Vector3D(0.0, 1.0, 0.0)) << std::endl;;

            if(models.size() > 0) BiovisionDisplay::draw(models[0], Vector3D(1.0, 0.0, 0.0), false);
            if(models.size() > 1) BiovisionDisplay::draw(models[1], Vector3D(0.0, 1.0, 0.0), false);
            if(models.size() > 2) BiovisionDisplay::draw(models[2], Vector3D(0.0, 0.0, 1.0), false);
            for(unsigned int i = 3; i < models.size(); i++)
                BiovisionDisplay::draw(models[i], Vector3D(uniform_rand(), uniform_rand(), uniform_rand()), false);

//            glTranslatef(20, 0.0, 0.0);
//            drawModel(model, Vector3D(0.0, 1.0, 0.0), 10.0, true, true);

        glPopMatrix();

    glPopMatrix();

    glutPostRedisplay();
    glutSwapBuffers( );


    if(elapsedTime > (1.0 / desiredFPS))
    {
        fps = 1.0 / elapsedTime;
        elapsedTime = 0.0;

        if(!freezeFrame)
            framenumber = (framenumber + 1) % animations.front().nframes();
    }

    string title("Motion: Frame " + num2str(framenumber%animations.front().nframes()) + " @ " + num2str(fps) + " fps");
    glutSetWindowTitle(title.c_str());
}

void setupWindows()
{
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(0, 0);
    mainID = glutCreateWindow("Biovision");
    glutDisplayFunc(draw_opengl);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    glutPassiveMotionFunc(motion);
    glutReshapeFunc(resize);
    glutIdleFunc(idle);
    glutSetCursor(GLUT_CURSOR_NONE);
    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

    glutSetWindow(mainID);
}

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        cout << "Usage:" << endl;
        cout << "BiovisionLoader [fps] [bvh_file_name]" << endl;
        exit(1);
    }

    desiredFPS = atof(argv[1]);
    if(desiredFPS <= 0.0)
    {
        std::cout << "Desired FPS " << desiredFPS << " is not greater than 0.0" << std::endl;
        exit(1);
    }

    for(int i = 2; i < argc; i++)
    {
        readers.push_back(BiovisionReader(string(argv[i])));
        animations.push_back(readers.back().animation());
        models.push_back(readers.back().model());
    }

    camera = QuakeCamera(Vector3D(), 0.0, 0.0, 300.0, 3.0, 1.0);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    setupWindows();

    glutMainLoop();
}
