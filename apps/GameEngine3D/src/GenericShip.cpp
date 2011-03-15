#include "GenericShip.h"


void GenericShip::init()
{
    forwardPower = 0.0;
    turnPower = 0.0;

    xPos = Thruster(Vector3D(0.0, 0.0, 6.0), Vector3D(0.0, -1.0, 0.0));
    xNeg = Thruster(Vector3D(0.0, 0.0, -6.0), Vector3D(0.0, -1.0, 0.0));
    yPos = Thruster(Vector3D(0.0, 0.0, 6.0), Vector3D(1.0, 0.0, 0.0));
    yNeg = Thruster(Vector3D(0.0, 0.0, -6.0), Vector3D(1.0, 0.0, 0.0));
    zPos = Thruster(Vector3D(3.0, 0.0, 0.0), Vector3D(0.0, 1.0, 0.0));
    zNeg = Thruster(Vector3D(-3.0, 0.0, 0.0), Vector3D(0.0, 1.0, 0.0));

    forward = Thruster(Vector3D(0.0, 0.0, -6.0), Vector3D(0.0, 0.0, 1.0));
}

GenericShip::GenericShip()
{
    linearResistance = 0.0;
    angularResistance = 0.0;

    init();
}

GenericShip::GenericShip(double mass, const Matrix3x3 &inertia, double linearResistance, double angularResistance)
        :RigidBody(mass, inertia)
{
    this->linearResistance = linearResistance;
    this->angularResistance = angularResistance;

    init();
}

GenericShip::~GenericShip()
{

}

void GenericShip::computeForceAndTorque(double time)
{
//resolve angular motions
    double actualTurnPower = turnPower;
    rotation = inertia * rotation; //scaled by mass distribution

    if (!(rotation.magnitude() > EPSILON)) //only calculate this if pilot is not trying to move, so stabilise motion
    {
        rotation = -angularMomentum;
        //stop jittering when ship have very small angular momentum
        if ((angularMomentum.magnitude() * 10.0) < turnPower) actualTurnPower = angularMomentum.magnitude() * 10.0;
    }

    if (rotation.magnitude() > EPSILON)
    {
        rotation.normalise();
        rotation *= (actualTurnPower / 2.0); //divide the power among the pairs of turn thrusters

        fireThruster(xPos, rotation[0]);
        fireThruster(xNeg, -(rotation[0]));
        fireThruster(yPos, rotation[1]);
        fireThruster(yNeg, -(rotation[1]));
        fireThruster(zPos, rotation[2]);
        fireThruster(zNeg, -(rotation[2]));
    }


//resolve linear motion
    if (movement.magnitude() > EPSILON)
    {
        movement.normalise();
        movement *= (forwardPower / 2.0); //divide the power among the movement thrusters

        fireThruster(forward, movement[2]);
    }

    //builtin slowdown factor
    force += (momentum * linearResistance);
    torque += (angularMomentum * angularResistance);

//cleanup for next iteration
    rotation = Vector3D();
    movement = Vector3D();
    direction = Vector3D(0.0, 0.0, 1.0);
}

void GenericShip::fireThruster(const Thruster& thruster, double power)
{
    applyBodyPointForce(thruster.getDirection() * power, thruster.getPosition());
}

double GenericShip::maxSpeed(double power)
{
    return power / ((-linearResistance) * mass);
}

double GenericShip::forwardPowerRequiredForSpeed(double speed)
{
    return -speed * mass * linearResistance;
}



//Thruster class
Thruster::Thruster()
{
    this->direction = Vector3D(0.0, 0.0, 1.0);
}

Thruster::Thruster(const Vector3D& position, const Vector3D& direction)
{
    this->position = position;
    this->direction = direction;
    this->direction.normalise();
}

Thruster::~Thruster()
{
}

const Vector3D& Thruster::getPosition() const
{
    return position;
}

const Vector3D& Thruster::getDirection() const
{
    return direction;
}
