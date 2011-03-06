#if defined _WIN64 || defined _WIN32
#include <windows.h>
#endif

#include "KinematicModel.h"
#include "Visualisation.h"

#include "AcclaimJoint.h"
#include "AcclaimReader.h"
#include "AcclaimAnimation.h"
#include "AcclaimDisplay.h"
#include "AcclaimMotion.h"

#include "QuakeCamera.h"
#include "MiscUtils.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <GL/glut.h>

using std::vector;
using std::cout;
using std::endl;
using std::string;
using std::ios;
using std::ofstream;


// Acclaim stuff
AcclaimModel model;
AcclaimReader reader;
AcclaimAnimation animation;

// control variables
QuakeCamera camera;
double lastTime = 0.0;
int framenumber = 0;
int mainID;
bool fixPointer = true;
bool freeze_frame = true;

// window stuff
int windowWidth = 640;
int windowHeight = 480;


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
                            framenumber = animation.nframes();
  			            break;
        case (int)('n'):freeze_frame = !freeze_frame;
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
    double realTime;
    double deltaTime;

    if(lastTime < 0.0)
        lastTime = getTime() - 0.01;

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

    realTime = getTime();
    deltaTime = realTime - lastTime;
    if(deltaTime > 0.02)
        deltaTime = 0.02;
    lastTime = realTime;

    setCamera(deltaTime);	//everything must be drawn after setting camera

    glPushMatrix();

        glPushMatrix();

            glColor3f(0.0, 1.0, 0.0);
            glBegin(GL_LINES);
                glNormal3f(0.0, 1.0, 0.0);
                for(int i = -50; i < 51; i+=10)
                {
                    glVertex3f(i, 0.0, -50.0);
                    glVertex3f(i, 0.0, 50.0);
                    glVertex3f(-50.0, 0.0, i);
                    glVertex3f(50.0, 0.0, i);
                }

                glColor3f(1.0, 0.0, 0.0);
                glVertex3f(0.0, 0.0, 0.0);
                glVertex3f(100.0, 0.0, 0.0);
                glColor3f(0.0, 1.0, 0.0);
                glVertex3f(0.0, 0.0, 0.0);
                glVertex3f(0.0, 100.0, 0.0);
                glColor3f(0.0, 0.0, 1.0);
                glVertex3f(0.0, 0.0, 0.0);
                glVertex3f(0.0, 0.0, 100.0);
            glEnd();

            model.setPose(animation.frames(framenumber % animation.nframes()));
//             std::cout << framenumber << "  " << computeAlignment(Quaternion(1.0, Vector3D()), model.root().local_orientation(), Vector3D(0.0, 1.0, 0.0)) << std::endl;;

            AcclaimDisplay::draw(model, Vector3D(1.0, 0.0, 0.0));

            glTranslatef(20, 0.0, 0.0);
            drawModel(model, Vector3D(0.0, 1.0, 0.0), 10.0, true, true);

        glPopMatrix();

    glPopMatrix();

    glutPostRedisplay();
    glutSwapBuffers( );

    char title[100];
    sprintf(title, "Motion: Frame %d", framenumber % animation.nframes());
    glutSetWindowTitle(title);

    if(!freeze_frame)
        framenumber++;
}

void setupWindows()
{
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(0, 0);
    mainID = glutCreateWindow("ASF_AMC");
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
        cout << "AcclaimLoader [asf_file_name] [amc_file_name]" << endl;
        exit(1);
    }

    reader = AcclaimReader(string(argv[1]));
    animation = AcclaimAnimation(string(argv[2]));
    model = reader.model();

    camera = QuakeCamera(Vector3D(), 0.0, 0.0, 30.0, 3.0, 1.0);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    setupWindows();

    glutMainLoop();
}
