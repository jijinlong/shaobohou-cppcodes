#include "ArcBall.h"

ArcBall::ArcBall()
{
    windowWidth = 0;
    windowHeight = 0;
}

ArcBall::ArcBall(unsigned int windowWidth, unsigned int windowHeight)
{
    this->windowWidth = windowWidth;
    this->windowHeight = windowHeight;
}

void ArcBall::setWindowSize(unsigned int windowWidth, unsigned int windowHeight)
{
    this->windowWidth = windowWidth;
    this->windowHeight = windowHeight;
}

void ArcBall::setSpherePosition(unsigned int xPos, unsigned int yPos)
{
    spherePosition = mapToSphere(xPos, yPos);
}

Vector3D ArcBall::mapToSphere(unsigned int xPos, unsigned int yPos) const
{
    Vector3D v;
    v[0] = (xPos - (0.5 * windowWidth)) / (0.5 * windowWidth);
    v[1] = -(yPos - (0.5 * windowHeight)) / (0.5 * windowHeight);
    v[2] = 0.0;

    if ((v * v) > 1.0)
        v.normalise();
    else
        v[2] = sqrt(1.0 - v * v);

    return v;
}

Quaternion ArcBall::drag(unsigned int xPos, unsigned int yPos)
{
    Vector3D newSpherePosition = mapToSphere(xPos, yPos);
    Vector3D axis = spherePosition ^ newSpherePosition;
    double angle = spherePosition * newSpherePosition;
    spherePosition = newSpherePosition;

    if ((axis * axis) > 0.000001)
        return Quaternion::makeFromAxisAngle(axis, angle);

    return Quaternion();
}
