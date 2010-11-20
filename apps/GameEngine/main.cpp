#include "main.h"

#include "special.h"
#include "collision.h"
#include "inertia.h"
#include "GraphicsMaths.h"

#include <cstdlib>

using std::vector;
using std::cout;
using std::endl;

bool drawHull = true;
unsigned int obbLevel = 0;

int main(int argc, char *argv[])
{
    first = QuatCamera(Vector3D(), Quaternion::makeFromEulerAngles(0.0, pi / 2.0, 0.0), 10.0, 3.0, 1.0);
    second = QuakeCamera(Vector3D(), 0.0, pi / 4.0, 10.0, 3.0, 1.0);
    current = &first;

//had to use pointer for testShip because otherwise, if use assignment, the RHS will be deleted from stack
//and consequently any other allocated objects(integrators) will be also be deallocated
//but the LHS still hold a copy so when it tries to use it, memmory error
//will need to overload = operator for deep copy for this to work
    testShip = new Object(Vector3D(), Quaternion(), 100.0, 1.0, 0.0, true, true);
    testShip->loadAC3D("TestShip.ac", 1.0);
    double mass = 10.0;
//testShip->pInfo.setInertia(getRectangularCylinderInertia(6.0, 3.0, 13.0, mass));
//testShip->pInfo.setInertia(getRectangularCylinderInertia(4.5, 4.5, 13.0, mass));
    testShip->pInfo.setInertia(getRectangularCylinderInertia(1.0, 20.0, 13.0, mass));
    testShip->pInfo.setMass(mass);

    testShip2 = new Object(Vector3D(0.0, 0.0, 20.0), Quaternion(), 100.0, 1.0, 0.0, true, true);
    testShip2->loadAC3D("TestShip2.ac", 1.0);
    testShip2->pInfo.setInertia(getRectangularCylinderInertia(1.0, 20.0, 13.0, mass));
    testShip2->pInfo.setMass(mass);

    currentShip = testShip;

    atexit(freeMemory);

    vector<Vector3D> polygon1;
    polygon1.push_back(Vector3D(0.0, 0.0, 0.0));
    polygon1.push_back(Vector3D(3.0, 0.0, 0.0));
    polygon1.push_back(Vector3D(3.0, 3.0, 0.0));
    polygon1.push_back(Vector3D(0.0, 3.0, 0.0));
    vector<Vector3D> polygon2;
    polygon2.push_back(Vector3D(2.0, 1.0, 0.0));
    polygon2.push_back(Vector3D(5.0, 1.0, 0.0));
    polygon2.push_back(Vector3D(5.0, 4.0, 0.0));
    polygon2.push_back(Vector3D(2.0, 4.0, 0.0));
    vector<Vector3D> intersection;
    polygonClipping(polygon2, polygon1, intersection);
    for (unsigned int i = 0; i < intersection.size(); i++)
        cout << i << " = " << intersection[i].toString() << endl;

//     cout << MOVE_UP << endl;
//     cout << TURN_UP << endl;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("SDL Test");
    glutDisplayFunc(draw_opengl);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutPassiveMotionFunc(motion);
    glutReshapeFunc(resize);

    glutSetCursor(GLUT_CURSOR_NONE);
    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

    glutMainLoop();
}

void freeMemory()
{
    delete testShip;
}

void keyboard(unsigned char key, int x, int y)
{
    current->move.set(key, true);
    currentShip->move.set(key, true);

    switch (key)
    {
        case 27:
            cout << "about to exit " << endl;
            exit(0);
        case '1':
            if (current == &first)
                current = &second;
            else
                current = &first;
            break;
//         case '2':
//             currentShip->update(RESET, true);
//             break;
        case '3':
            followCam = !followCam;
            break;
        case '4':
            if (currentShip == testShip)
                currentShip = testShip2;
            else if (currentShip == testShip2)
                currentShip = testShip;
            break;
        case '0':
            currentShip->pInfo.applyBodyImpulse(Vector3D(0.0, 100.0, 0.0), Vector3D(0.0, 0.0, 5.0));
            break;
        case ',':
            drawHull = !drawHull;
            break;
        case '.':
            obbLevel++;
            break;
//         case 'k':
//             currentShip->update(TURN_UP, true);
//             break;
//         case 'i':
//             currentShip->update(TURN_DOWN, true);
//             break;
//         case 'j':
//             currentShip->update(TURN_LEFT, true);
//             break;
//         case 'l':
//             currentShip->update(TURN_RIGHT, true);
//             break;
//         case 'u':
//             currentShip->update(ROLL_CC, true);
//             break;
//         case 'o':
//             currentShip->update(ROLL_C, true);
//             break;
//         case 't':
//             currentShip->update(MOVE_FORWARD, true);
//             break;
//         case 'g':
//             currentShip->update(MOVE_BACKWARD, true);
//             break;
        default:
            break;
    }
}

void keyboardUp(unsigned char key, int x, int y)
{
    current->move.set(key, false);
    currentShip->move.set(key, false);

    switch (key)
    {
        case 27:
            cout << "about to exit " << endl;
            exit(0);
        default:
            break;
    }
}

void motion(int x, int y)
{
    double diffX = (double)(x - (windowWidth / 2.0))/ (double)windowWidth;
    double diffY = (double)(windowHeight - y - (windowHeight / 2.0))/ (double)windowHeight;
    current->modOrientation(diffX, diffY);

    if (((x - windowWidth / 2) != 0) || ((y - windowHeight / 2) != 0))
        glutWarpPointer(windowWidth / 2, windowHeight / 2);
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
    Vector3D velocity = current->update(realTime);
    current->setPosition(current->getPosition() + velocity);
    current->setPerspective();
}

void draw_opengl()
{frame++;
    double realTime;
    double deltaTime;

    if (lastTime < 0.0)
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
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glColorMaterial(GL_FRONT, GL_DIFFUSE | GL_SPECULAR);

    GLfloat        lightPos[] = {0.0, 0.0, 0.0, 1.0},
                                white[] = {1.0, 1.0, 1.0, 1.0};
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);

    vector<double> lambdas;
    vector<int> aCoordsIndex;
    vector<int> bCoordsIndex;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glLoadIdentity();

    realTime = getTime();
    deltaTime = realTime - lastTime;
    if (deltaTime > 0.02) deltaTime = 0.02;
    lastTime = realTime;

    testShip->updateView(deltaTime);
    testShip2->updateView(deltaTime);

    Vector3D d = testShip->pInfo.getPosition() - testShip2->pInfo.getPosition();

    if (OBB::testIntersection(testShip->tree.getOBB(), testShip2->tree.getOBB()))
    {
        testShip->obbColour = Vector3D(1.0, 0.0, 0.0);
        testShip2->obbColour = Vector3D(1.0, 0.0, 0.0);

        lambdas.clear();
        aCoordsIndex.clear();
        bCoordsIndex.clear();
        seed = GJK(testShip->tree.getConvexHull(), testShip2->tree.getConvexHull(), seed, lambdas, aCoordsIndex, bCoordsIndex);

        if ((seed * seed) < 0.0001)
        {cout << "Collided " << frame << endl;

            testShip->pInfo.revertState();
            testShip2->pInfo.revertState();

            testShip->tree.sync(testShip->pInfo.getPosition(), testShip->pInfo.getOrientation());
            testShip2->tree.sync(testShip2->pInfo.getPosition(), testShip2->pInfo.getOrientation());

            lambdas.clear();
            aCoordsIndex.clear();
            bCoordsIndex.clear();
            seed = GJK(testShip->tree.getConvexHull(), testShip2->tree.getConvexHull(), seed, lambdas, aCoordsIndex, bCoordsIndex);
            cout << "reverted seed = " << seed.toString() << endl;
            Vector3D a, b;
            for (unsigned int i = 0; i < lambdas.size(); i++)
            {cout << "indices drawn = " << aCoordsIndex[i] << " " << bCoordsIndex [i] << " with " << lambdas[i] << endl;
                a += (testShip->tree.getConvexHull().getOrientation().rotate(testShip->tree.getConvexHull().vertex(aCoordsIndex[i]).position) + testShip->tree.getConvexHull().getPosition()) * lambdas[i];
                b +=(testShip2->tree.getConvexHull().getOrientation().rotate(testShip2->tree.getConvexHull().vertex(bCoordsIndex[i]).position) + testShip2->tree.getConvexHull().getPosition()) * lambdas[i];
            }
            cout << "a = " << a.toString() << endl;
            cout << "b = " << b.toString() << endl;
            Vector3D point = (a + b) / 2.0;
            Vector3D normal = a - b;
            normal.normalise();
            cout << "point = " << point.toString() << endl;
            cout << "normal = " << normal.toString() << endl;
            if ((normal * normal) < 0.0001)
            {
                cout << "bah, normal is wrong" << endl;
                exit(1);
            }

            /*
            Vector3D point = (testShip->pInfo.getPosition() + testShip2->pInfo.getPosition()) / 2.0;
            Vector3D normal = testShip->pInfo.getPosition() - testShip2->pInfo.getPosition();
            normal.normalise();
            */

            cout << "before impulse" << endl;
            cout << testShip->pInfo.getMomentum().toString() << endl;
            cout << testShip->pInfo.getAngularMomentum().toString() << endl;
            cout << testShip2->pInfo.getMomentum().toString() << endl;
            cout << testShip2->pInfo.getAngularMomentum().toString() << endl;

            impulseCollision(testShip->pInfo, testShip2->pInfo, point, normal, 1.0, 0.0);

            cout << "after impulse" << endl;
            cout << testShip->pInfo.getMomentum().toString() << endl;
            cout << testShip->pInfo.getAngularMomentum().toString() << endl;
            cout << testShip2->pInfo.getMomentum().toString() << endl;
            cout << testShip2->pInfo.getAngularMomentum().toString() << endl;

        }
        /*
        if(OBB::testIntersection(testShip->obb, testShip2->obb))
        {
         cout << "BAH" << endl;
         exit(1);
        }*/


    }
    else
    {
        testShip->obbColour = Vector3D(0.0, 1.0, 0.0);
        testShip2->obbColour = Vector3D(0.0, 1.0, 0.0);
    }

    if (followCam)
    {
        first.setPosition(currentShip->pInfo.getPosition());
        first.setOrientation(currentShip->pInfo.getOrientation());
    }
    setCamera(deltaTime);	//everything must be drawn after setting camera

    glPushMatrix();
    glTranslatef(sin(realTime * 2.0) * 25.6, -8.0, cos(realTime * 2.0) * 25.6);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glPopMatrix();

    glPushMatrix();
    glBegin(GL_QUADS);
    glColor4f(0.0, 0.0, 1.0, 1.0);
    glNormal3f(0.0, 1.0, 0.0);
    glVertex3f(-51.2, -10.0, -51.2);
    glVertex3f(-51.2, -10.0, 51.2);
    glVertex3f(51.2, -10.0, 51.2);
    glVertex3f(51.2, -10.0, -51.2);
    glEnd();
    testShip->draw(drawHull, obbLevel);
    testShip2->draw(drawHull, obbLevel);
    glPopMatrix();

//cout << "seed = " << seed.toString() << endl;
    lambdas.clear();
    aCoordsIndex.clear();
    bCoordsIndex.clear();
    seed = GJK(testShip->tree.getConvexHull(), testShip2->tree.getConvexHull(), seed, lambdas, aCoordsIndex, bCoordsIndex);
    Vector3D a, b;
    vector<Vector3D> features;
    vector<Vector3D> planeFeatures;
    vector<Vector3D> newFeatures;
    for (unsigned int i = 0; i < lambdas.size(); i++)
    {//cout << "indices drawn = " << aCoordsIndex[i] << " " << bCoordsIndex [i] << endl;
        a += (testShip->tree.getConvexHull().getOrientation().rotate(testShip->tree.getConvexHull().vertex(aCoordsIndex[i]).position) + testShip->tree.getConvexHull().getPosition()) * lambdas[i];
        b +=(testShip2->tree.getConvexHull().getOrientation().rotate(testShip2->tree.getConvexHull().vertex(bCoordsIndex[i]).position) + testShip2->tree.getConvexHull().getPosition()) * lambdas[i];

        Vector3D n = Vector3D(0.0, 0.0, -1.0);

        features.clear();
        planeFeatures.clear();
        testShip2->tree.getConvexHull().getTransformedSupportFeature(features, n);
        n.normalise();

        if (features.size() > 1)
        {
            Vector3D origin, xAxis, yAxis;
            computePlane(features, n, origin, xAxis, yAxis);
            planeMap(features, origin, xAxis, yAxis, planeFeatures);
            inversePlaneMap(planeFeatures, origin, xAxis, yAxis, newFeatures);
        }
        /*
        for(int j = 0; j < features.size(); j++)
        cout << "features = " << features[j].toString() << endl;
        for(int j = 0; j < planeFeatures.size(); j++)
        cout << "planeFeatures = " << planeFeatures[j].toString() << endl;
        for(int j = 0; j < newFeatures.size(); j++)
        cout << "newFeatures = " << newFeatures[j].toString() << endl;*/
    }

    ConvexHull2D hull;
    if (planeFeatures.size() > 1)
        hull = ConvexHull2D(planeFeatures);

    glPushMatrix();
    glDisable(GL_LIGHTING);
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(a[0], a[1], a[2]);
    glVertex3f(b[0], b[1], b[2]);
    glEnd();
    glColor3f(1.0, 1.0, 1.0);
    Vector3D pos = testShip2->pInfo.getPosition();
    Quaternion ori = testShip2->pInfo.getOrientation();
    glTranslatef(pos[0], pos[1] + 2.0, pos[2]);
    glRotatef(rad2deg(ori.getAngle()), ori.getAxis()[0], ori.getAxis()[1], ori.getAxis()[2]);
    glBegin(GL_LINE_LOOP);
    for (unsigned int i = 0; i < hull.getVertices().size(); i++)
    {int size = hull.getVertices().size();
        int index = hull.getVertices()[i]->index;
        //cout << hull.getVertices().size() << endl;
        glColor3f((size - i)/ (double)size, 0.0, i / (double)size);
        glVertex3f(features[index][0], features[index][1], features[index][2]);
    }
    glEnd();
    glEnable(GL_LIGHTING);
    glPopMatrix();

    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(10000.0, 0.0, 0.0);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 10000.0, 0.0);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 10000.0);
    glEnd();

    glutPostRedisplay();
    glutSwapBuffers();

}
