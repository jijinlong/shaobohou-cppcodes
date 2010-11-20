#ifndef INERTIA_H
#define INERTIA_H

#include "Quaternion.h"
#include "Matrix3x3.h"

//calculate 3x3 matrix of inertia for various shapes in 3d
Matrix3x3 getRectangularCylinderInertia(double x, double y, double z, double mass);
Matrix3x3 getCircularCylinderInertia(double r, double l, double mass);  //assume aligned along the y axis, i.e l is along y axis
Matrix3x3 getSphereInertia(double radius, double mass);

//calculate the globla inverse inertia of an objets inertia
Matrix3x3 getGlobalInertia(const Quaternion &orientation, const Matrix3x3 &inertia);

#endif
