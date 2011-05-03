#include "CrossPlatform.h"

#include "MiscUtils.h"
#include "QuakeCamera.h"
#include "Mesh.h"
#include "PlyReader.h"
#include "TimeFuncs.h"

#include <vector>
#include <fstream>
#include <iostream>

#include <GL/glut.h>

using std::cout;
using std::endl;
using std::string;

Mesh faceMesh;


// variables for controlling the app
QuakeCamera camera;
int framenumber = 0;
bool fixPointer = false;
bool freezeFrame = false;
// window size
int mainID = -1;
int windowWidth = 640;
int windowHeight = 480;
// frame rate control
double desiredFPS = 60.0;
double elapsedTime = 0.0;
double lastTime = -1.0;
double fps = 0.0;



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void idle()
{
}

void keyboard(unsigned char key, int x, int y)
{
    if(camera.move.set(key, true))
        return;

    int tempID = glutGetWindow();
    switch (key)
    {
        case 27:
            cout << "about to exit " << endl;
            exit(0);
            break;
        case (int)('f'):	fixPointer = !fixPointer;
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
        case (int)('b'):	framenumber--;
            break;
        case (int)('n'):    freezeFrame = !freezeFrame;
  			                break;
        case (int)('m'):	framenumber++;
            break;
        case (int)('='):    camera.setRunSpeed(camera.getRunSpeed()*2.0);
            break;
        case (int)('-'):    camera.setRunSpeed(camera.getRunSpeed()*0.5);
            break;
        default:
            break;
    }
}

void keyboardUp(unsigned char key, int x, int y)
{
    if(camera.move.set(key, false))
        return;
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

void drawMesh(const Mesh &mesh)
{
    //glColor3f(0.0, 1.0, 0.0);
    glVertexPointer(3, GL_DOUBLE, 0, mesh.getVertices().vals());
    glColorPointer(3, GL_DOUBLE, 0, mesh.getColours().vals());
    glNormalPointer(GL_DOUBLE, 0, mesh.getVertexNormals().vals());
    if(mesh.getFaces().dim1() == 3)
        glDrawElements(GL_TRIANGLES, mesh.getFaces().dim2()*3,  GL_UNSIGNED_INT, mesh.getFaces().vals());
    else if(mesh.getFaces().dim1() == 4)
        glDrawElements(GL_QUADS, mesh.getFaces().dim2()*4,  GL_UNSIGNED_INT, mesh.getFaces().vals());
    else
        assert(false);
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

    glPushMatrix();
        glRotatef(-90.0, 0.0, 0.0, 1.0);
        drawMesh(faceMesh);
    glPopMatrix();

    glutPostRedisplay();
    glutSwapBuffers( );

    // simple method for controlling speed of animation
    if(elapsedTime > (1.0 / desiredFPS))
    {
        fps = 1.0 / elapsedTime;
        elapsedTime = 0.0;

        if(!freezeFrame)
            framenumber++;
    }

    string title("OpenGL Template: Frame " + num2str(framenumber) + " @ " + num2str(fps) + "fps");
    glutSetWindowTitle(title.c_str());
}

void setupWindows()
{
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(0, 0);
    mainID = glutCreateWindow("PlayIKData");
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
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    camera = QuakeCamera(Vector3D(0.0, 0.0, -2.0), 0.0, 0.0, 800.0, 3.0, 1.0);

    if(argc < 2)
    {
        cout << "Usage:" << endl;
        cout << "PlyReader <filename> --OPTIONAL-- <framerate>" << endl;
    }

    if (argc > 2)
        desiredFPS = atof(argv[2]);

    string filename(argv[1]);
    PlyFile plyFile(filename);
    //plyFile.printData(cout);

    cout << endl;

    faceMesh = Mesh(plyFile);
    faceMesh.print(cout);

    setupWindows();
    glutMainLoop();
}
