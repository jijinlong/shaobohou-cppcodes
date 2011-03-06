#if defined _WIN64 || defined _WIN32
#include <windows.h>
#endif

#include "GLUtils.h"

#include "special.h"

#include <GL/glu.h>
#include <fstream>

using std::string;
using std::ofstream;
using std::ios;
using std::endl;

void glColor(const Vector3D &c)
{
    glColor3d(c[0], c[1], c[2]);
}

void glColor(const Vector3D &c, double alpha)
{
    glColor4d(c[0], c[1], c[2], alpha);
}

void glVertex(const Vector3D &v)
{
    glVertex3d(v[0], v[1], v[2]);
}

void glNormal(const Vector3D &v)
{
    glNormal3d(v[0], v[1], v[2]);
}

void glTranslate(const Vector3D &t)
{
    glTranslated(t[0], t[1], t[2]);
}

void glRotate(const Quaternion &R)
{
    Vector3D axis = R.getAxis();
    double angle = R.getAngle();
    glRotated(rad2deg(angle), axis[0], axis[1], axis[2]);
}

void setIdentityModelView()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void takeScreenShot(const string &filename)
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    unsigned int width = viewport[2];
    unsigned int height = viewport[3];

    ofstream gf(filename.c_str(), ios::binary);
    gf << "P6" << endl << width << '\t' << height << endl << "255" << endl;
    gf.close();
    FILE *ff= fopen(filename.c_str(),"ab");
    GLubyte *v = new GLubyte[width * height * 3];

    glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE, v);

    int status = 0;
    for (unsigned int j = 0; j < height; j++)
        status = fwrite(&v[(height-j-1)*width*3],width*3,sizeof(GLbyte),ff);

    glDrawPixels(width, height, GL_RGB, GL_BYTE, v);

    delete v;
    fclose(ff);
}

void castRayFromPixel(Vector3D &rayOrigin, Vector3D &rayDirection, double trueX, double trueY)
{
    int view[4];
    double model[16], proj[16];
    glGetIntegerv(GL_VIEWPORT, view);
    glGetDoublev(GL_MODELVIEW_MATRIX, model);
    glGetDoublev(GL_PROJECTION_MATRIX, proj);

    gluUnProject(trueX, trueY, 0.0, model, proj, view, &(rayOrigin[0]), &(rayOrigin[1]), &rayOrigin[2]);
    gluUnProject(trueX, trueY, 1.0, model, proj, view, &(rayDirection[0]), &(rayDirection[1]), &rayDirection[2]);

    rayDirection = rayDirection - rayOrigin;
    rayDirection.normalise();
}
