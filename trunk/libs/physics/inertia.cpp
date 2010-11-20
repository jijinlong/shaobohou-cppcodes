#include "inertia.h"

Matrix3x3 getRectangularCylinderInertia(double x, double y, double z, double mass)
{
    return Matrix3x3((1.0 / 12.0) * mass * (y * y + z * z), 0.0, 0.0,
                     0.0, (1.0 / 12.0) * mass * (x * x + z * z), 0.0,
                     0.0, 0.0, (1.0 / 12.0) * mass * (x * x + y * y));
}

Matrix3x3 getCircularCylinderInertia(double r, double l, double mass)
{
    double t = ((1.0 / 4.0) * mass * r * r) + ((1.0 / 12.0) * mass * l * l);

    return Matrix3x3(t, 0.0, 0.0, 0.0, (1.0 / 2.0) * mass * r * r, 0.0, 0.0, 0.0, t);
}

Matrix3x3 getSphereInertia(double radius, double mass)
{
    double t = (2.0 / 5.0) * mass * radius * radius;
    return Matrix3x3(t, 0.0, 0.0, 0.0, t, 0.0, 0.0, 0.0, t);
}

Matrix3x3 getGlobalInertia(const Quaternion &orientation, const Matrix3x3 &inertia)
{
    Matrix3x3   o = orientation.makeRotationMatrix(),
                  oT = o.transpose();

    return o * inertia * oT;
}
