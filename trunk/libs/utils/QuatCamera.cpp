#include "QuatCamera.h"

QuatCamera::QuatCamera()
{
    modOrientation(0.0, 0.0, 0.0);
}

QuatCamera::QuatCamera(const Vector3D &pos, const Quaternion &orientation, double runSpeed, double turnSpeed, double sensitivity)
        :BasicCamera(pos, runSpeed, turnSpeed, sensitivity)
{
    this->orientation = orientation;
    modOrientation(0.0, 0.0, 0.0);
}

QuatCamera::~QuatCamera()
{
}

void QuatCamera::setOrientation(const Quaternion &orientation)
{
    this->orientation = orientation;
    this->trueOrientation = orientation * cameraOffset;
}

void QuatCamera::modOrientation(double rotateX, double rotateY, double rotateZ)
{
    Quaternion temp = Quaternion::makeFromEulerAngles(rotateX, rotateY, rotateZ);
    setOrientation(getOrientation() * ~temp);
}

void QuatCamera::modOrientation(double changeX, double changeY)
{
    modOrientation(changeY * sensitivity, changeX * sensitivity, 0.0);
}

void QuatCamera::reset()
{
    setOrientation(Quaternion());
    setPosition(Vector3D());
}
