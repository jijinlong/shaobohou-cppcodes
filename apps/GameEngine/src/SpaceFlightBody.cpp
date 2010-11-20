#include "SpaceFlightBody.h"

void SpaceFlightBody::init()
{
}

SpaceFlightBody::SpaceFlightBody()
        :RigidBody()
{
    linearResistance = 0.0;
    angularResistance = 0.0;

    init();
}

SpaceFlightBody::SpaceFlightBody(double mass, const Matrix3x3 &inertia, double linearResistance, double angularResistance)
        :RigidBody(mass, inertia)
{
    this->linearResistance = linearResistance;
    this->angularResistance = angularResistance;

    init();
}

SpaceFlightBody::~SpaceFlightBody()
{
}

void SpaceFlightBody::computeForceAndTorque(double time)
{
    force += (momentum * linearResistance);
    torque += (angularMomentum * angularResistance);
}
