#ifndef GL_UTILS_H
#define GL_UTILS_H

#include <string>

#include "Vector3D.h"
#include "Quaternion.h"

void glColor(const Vector3D &c);
void glColor(const Vector3D &c, double alpha);
void glVertex(const Vector3D &v);
void glNormal(const Vector3D &v);
void glTranslate(const Vector3D &t);
void glRotate(const Quaternion &R);

void setIdentityModelView();

void takeScreenShot(const std::string &filename);
void castRayFromPixel(Vector3D &rayOrigin, Vector3D &rayDirection, double trueX, double trueY);

#endif
