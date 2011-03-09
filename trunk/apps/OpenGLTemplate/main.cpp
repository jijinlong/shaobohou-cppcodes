#include "CrossPlatform.h"

#include <GL/glut.h>

#include "QuakeCamera.h"
#include "QuatCamera.h"
#include "MiscUtils.h"
#include "GLUtils.h"

#include <vector>
#include <fstream>
#include <iostream>
#include <cstdlib>

using std::cout;
using std::endl;



// variables for controlling the app
QuakeCamera quakeCamera(Vector3D(0.0, 0.0, -20.0), 0.0, 0.0, 10.0, 3.0, 1.0);
QuatCamera quatCamera(Vector3D(0.0, 0.0, -20.0), Quaternion(), 10.0, 3.0, 1.0);
BasicCamera &camera = quakeCamera;
int framenumber = 0;
bool fixPointer = false;
bool freezeFrame = false;
// window size
int windowWidth = 640;
int windowHeight = 480;
int mainID = 0;
// frame rate control
double desiredFPS = 60.0;
double elapsedTime = 0.0;
double lastTime = -1.0;
double fps = 0.0;
// screenshot
bool capturing = false;



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
        case (int)('f'):fixPointer = !fixPointer;
                        tempID = glutGetWindow();
                        if (fixPointer)
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
        case (int)('b'):framenumber--;
                        break;
        case (int)('n'):freezeFrame = !freezeFrame;
                        break;
        case (int)('m'):framenumber++;
                        break;
        case (int)('c'):capturing = !capturing;
                        break;
        default:        camera.move.set(key, true);
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
        case (int)('m'):
        case (int)('c'):break;
        default:        camera.move.set(key, false);
                        break;
    }
}

void motion(int x, int y)
{
    if (fixPointer)
    {
        camera.modOrientation((double)(x - (windowWidth / 2.0))/ (double)windowWidth, (double)(windowHeight - y - (windowHeight / 2.0))/ (double)windowHeight);

        if (((x - windowWidth / 2) != 0) || ((y - windowHeight / 2) != 0))
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
    if (lastTime < 0.0)
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

    // glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glEnable(GL_NORMALIZE);

    GLfloat        lightPos[] = {0.0, 0.0, 0.0, 1.0}, white[] = {1.0, 1.0, 1.0, 1.0};

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    setCamera(deltaTime);	// everything must be drawn after setting camera


    // draw a cube
    glPushMatrix();
        glTranslatef(-1.5f,0.0f,-6.0f);                     // Move Left 1.5 Units And Into The Screen 6.0
        glBegin(GL_TRIANGLES);                              // Drawing Using Triangles
            glColor3f(1.0f,0.0f,0.0f);          // Red
            glVertex3f( 0.0f, 1.0f, 0.0f);          // Top Of Triangle (Front)
            glColor3f(0.0f,1.0f,0.0f);          // Green
            glVertex3f(-1.0f,-1.0f, 1.0f);          // Left Of Triangle (Front)
            glColor3f(0.0f,0.0f,1.0f);          // Blue
            glVertex3f( 1.0f,-1.0f, 1.0f);          // Right Of Triangle (Front)
            glColor3f(1.0f,0.0f,0.0f);          // Red
            glVertex3f( 0.0f, 1.0f, 0.0f);          // Top Of Triangle (Right)
            glColor3f(0.0f,0.0f,1.0f);          // Blue
            glVertex3f( 1.0f,-1.0f, 1.0f);          // Left Of Triangle (Right)
            glColor3f(0.0f,1.0f,0.0f);          // Green
            glVertex3f( 1.0f,-1.0f, -1.0f);         // Right Of Triangle (Right)
            glColor3f(1.0f,0.0f,0.0f);            // Red
            glVertex3f( 0.0f, 1.0f, 0.0f);          // Top Of Triangle (Back)
            glColor3f(0.0f,1.0f,0.0f);          // Green
            glVertex3f( 1.0f,-1.0f, -1.0f);         // Left Of Triangle (Back)
            glColor3f(0.0f,0.0f,1.0f);          // Blue
            glVertex3f(-1.0f,-1.0f, -1.0f);         // Right Of Triangle (Back)
            glColor3f(1.0f,0.0f,0.0f);          // Red
            glVertex3f( 0.0f, 1.0f, 0.0f);          // Top Of Triangle (Left)
            glColor3f(0.0f,0.0f,1.0f);          // Blue
            glVertex3f(-1.0f,-1.0f,-1.0f);          // Left Of Triangle (Left)
            glColor3f(0.0f,1.0f,0.0f);          // Green
            glVertex3f(-1.0f,-1.0f, 1.0f);          // Right Of Triangle (Left)
        glEnd();                                            // Finished Drawing The Triangle
    glPopMatrix();

    glPushMatrix();                   // Reset The Current Modelview Matrix
        glTranslatef(1.5f,0.0f,-6.0f);              // Move Right 1.5 Units And Into The Screen 6.0
        glColor3f(0.5f,0.5f,1.0f);                          // Set The Color To Blue One Time Only
        glBegin(GL_QUADS);                                  // Draw A Quad
            glColor3f(0.0f,1.0f,0.0f);          // Set The Color To Blue
            glVertex3f( 1.0f, 1.0f,-1.0f);          // Top Right Of The Quad (Top)
            glVertex3f(-1.0f, 1.0f,-1.0f);          // Top Left Of The Quad (Top)
            glVertex3f(-1.0f, 1.0f, 1.0f);          // Bottom Left Of The Quad (Top)
            glVertex3f( 1.0f, 1.0f, 1.0f);          // Bottom Right Of The Quad (Top)
            glColor3f(1.0f,0.5f,0.0f);          // Set The Color To Orange
            glVertex3f( 1.0f,-1.0f, 1.0f);          // Top Right Of The Quad (Bottom)
            glVertex3f(-1.0f,-1.0f, 1.0f);          // Top Left Of The Quad (Bottom)
            glVertex3f(-1.0f,-1.0f,-1.0f);          // Bottom Left Of The Quad (Bottom)
            glVertex3f( 1.0f,-1.0f,-1.0f);          // Bottom Right Of The Quad (Bottom)
            glColor3f(1.0f,0.0f,0.0f);          // Set The Color To Red
            glVertex3f( 1.0f, 1.0f, 1.0f);          // Top Right Of The Quad (Front)
            glVertex3f(-1.0f, 1.0f, 1.0f);          // Top Left Of The Quad (Front)
            glVertex3f(-1.0f,-1.0f, 1.0f);          // Bottom Left Of The Quad (Front)
            glVertex3f( 1.0f,-1.0f, 1.0f);          // Bottom Right Of The Quad (Front)
            glColor3f(1.0f,1.0f,0.0f);          // Set The Color To Yellow
            glVertex3f( 1.0f,-1.0f,-1.0f);          // Bottom Left Of The Quad (Back)
            glVertex3f(-1.0f,-1.0f,-1.0f);          // Bottom Right Of The Quad (Back)
            glVertex3f(-1.0f, 1.0f,-1.0f);          // Top Right Of The Quad (Back)
            glVertex3f( 1.0f, 1.0f,-1.0f);          // Top Left Of The Quad (Back)
            glColor3f(0.0f,0.0f,1.0f);          // Set The Color To Blue
            glVertex3f(-1.0f, 1.0f, 1.0f);          // Top Right Of The Quad (Left)
            glVertex3f(-1.0f, 1.0f,-1.0f);          // Top Left Of The Quad (Left)
            glVertex3f(-1.0f,-1.0f,-1.0f);          // Bottom Left Of The Quad (Left)
            glVertex3f(-1.0f,-1.0f, 1.0f);          // Bottom Right Of The Quad (Left)
            glColor3f(1.0f,0.0f,1.0f);          // Set The Color To Violet
            glVertex3f( 1.0f, 1.0f,-1.0f);          // Top Right Of The Quad (Right)
            glVertex3f( 1.0f, 1.0f, 1.0f);          // Top Left Of The Quad (Right)
            glVertex3f( 1.0f,-1.0f, 1.0f);          // Bottom Left Of The Quad (Right)
            glVertex3f( 1.0f,-1.0f,-1.0f);          // Bottom Right Of The Quad (Right)
    glEnd();                        // Done Drawing The Quad
    glPopMatrix();



    glutPostRedisplay();
    glutSwapBuffers( );

    // simple method for controlling speed of animation
    if (elapsedTime > (1.0 / desiredFPS))
    {
        fps = 1 / elapsedTime;
        elapsedTime = 0.0;

        if (!freezeFrame)
        {
            if (capturing)
                takeScreenShot("screenshot.ppm");
        }
    }

    char title[100];
    sprintf(title, "OpenGL Template: Frame %d @ %f fps", framenumber, fps);
    glutSetWindowTitle(title);
}

void setupWindows()
{
    glutInitWindowSize(windowWidth, windowHeight);
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
    glutSetCursor(GLUT_CURSOR_NONE);
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

    if (argc > 1)
        desiredFPS = atof(argv[1]);

    setupWindows();
    glutMainLoop();
}
