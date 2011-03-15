#include "BasicCamera.h"

#include "GLUtils.h"


const Quaternion BasicCamera::cameraOffset = Quaternion::makeFromEulerAngles(0.0, PI, 0.0);


BasicCamera::BasicCamera()
{
    this->runSpeed = 0.0;
    this->turnSpeed = 0.0;
    this->sensitivity = 0.0;
}

BasicCamera::BasicCamera(const Vector3D &position, double runSpeed, double turnSpeed, double sensitivity)
{
    this->position = position;
    this->runSpeed = runSpeed;
    this->turnSpeed = turnSpeed;
    this->sensitivity = sensitivity;
}

BasicCamera::~BasicCamera()
{
}

void BasicCamera::setPosition(const Vector3D &position)
{
    this->position = position;
}

const Vector3D& BasicCamera::getPosition() const
{
    return position;
}

const Quaternion& BasicCamera::getOrientation() const
{
    return orientation;
}

double BasicCamera::getRunSpeed() const
{
    return runSpeed;
}

double BasicCamera::getTurnSpeed() const
{
    return turnSpeed;
}

void BasicCamera::setRunSpeed(double newRunSpeed)
{
    runSpeed = newRunSpeed;
}

void BasicCamera::setTurnSpeed(double newTurnSpeed)
{
    turnSpeed = newTurnSpeed;
}

void BasicCamera::setPerspective()
{
    setIdentityModelView();

    Quaternion cameraOrientation = ~trueOrientation;
    Vector3D cameraTranslation = -position;

    glRotate(cameraOrientation);
    glTranslate(cameraTranslation);
}

Vector3D BasicCamera::update(double deltaTime)
{
    double deltaRotX = 0.0, deltaRotY = 0.0, deltaRotZ = 0.0;
    double deltaX = 0.0, deltaY = 0.0, deltaZ = 0.0;

    double tTurn = turnSpeed * deltaTime;
    double tRun = runSpeed * deltaTime;

    //updates for pos
    if (move.forward) deltaZ += tRun;
    if (move.backward) deltaZ -= tRun;
    if (move.leftward) deltaX += tRun;
    if (move.rightward) deltaX -= tRun;
    if (move.upward) deltaY += tRun;
    if (move.downward) deltaY -= tRun;

    //update parameters for orientation
    if (move.rotXPos) deltaRotX += tTurn;
    if (move.rotXNeg) deltaRotX -= tTurn;
    if (move.rotYPos) deltaRotY += tTurn;
    if (move.rotYNeg) deltaRotY -= tTurn;
    if (move.rotZPos) deltaRotZ += tTurn;
    if (move.rotZNeg) deltaRotZ -= tTurn;

    if (move.rotXPos || move.rotXNeg || move.rotYPos || move.rotYNeg || move.rotZPos || move.rotZNeg)
        modOrientation(deltaRotX, deltaRotY, deltaRotZ);

    if (move.forward || move.backward || move.upward || move.downward || move.leftward || move.rightward)
        return orientation.rotate(Vector3D(deltaX, deltaY, deltaZ));

    return Vector3D();
}
