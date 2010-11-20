#include "RigidBody.h"

#include "special.h"

#include <cassert>

void RigidBody::init()
{
    integrator = Integrator(13);

    inverseMass = 1.0 / mass;
    inverseInertia = inertia.inverse();

    oldstates = state2array();
}

void RigidBody::computeDerivedStates()
{
    velocity = inverseMass * momentum;
    angularVelocity = inverseInertia * angularMomentum;
}

RigidBody::RigidBody()
{
    mass = 1.0;
    inertia = Matrix3x3::identity();

    init();
}

RigidBody::RigidBody(double mass, const Matrix3x3 &inertia)
{
    this->mass = mass;
    this->inertia = inertia;

    init();
}

RigidBody::~RigidBody()
{
}

void RigidBody::step(double time, double deltaTime)
{
    computeForceAndTorque(time);
    std::vector<double> dy = integrator.integrateByRungeKutta(time, deltaTime, *this);

    //once collision detection is done, the following will not be called here, dys will be used to help collision detection
    std::vector<double> y = state2array();
    oldstates = y;
    for (unsigned int i = 0; i < integrator.size(); i++) y[i] += dy[i];
    array2state(y);

    force = Vector3D();
    torque = Vector3D();
}

std::vector<double> RigidBody::derive(double t, const std::vector<double> &y) const
{
    assert(y.size() == 13);

    std::vector<double> dydt(13);

    //force as derivative of momentum
    dydt[0] = force[0];
    dydt[1] = force[1];
    dydt[2] = force[2];

    //velocity as derivative of position, = momentum / mass
    dydt[3] = y[0] / mass;
    dydt[4] = y[1] / mass;
    dydt[5] = y[2] / mass;

    //torque as derivative of momentum
    dydt[6] = torque[0];
    dydt[7] = torque[1];
    dydt[8] = torque[2];

    Quaternion tempOrientation(y[9],y[10], y[11], y[12]);
    Vector3D av = tempOrientation.rotate(inverseInertia * angularMomentum);
    Quaternion orientationDerivative = 0.5 * av * tempOrientation;

    dydt[9] = orientationDerivative.getScalar();
    dydt[10] = orientationDerivative.getVector()[0];
    dydt[11] = orientationDerivative.getVector()[1];
    dydt[12] = orientationDerivative.getVector()[2];

    return dydt;
}

std::vector<double> RigidBody::state2array() const
{
    std::vector<double> y(13);

    y[0] = momentum[0];
    y[1] = momentum[1];
    y[2] = momentum[2];
    y[3] = position[0];
    y[4] = position[1];
    y[5] = position[2];

    y[6] = angularMomentum[0];
    y[7] = angularMomentum[1];
    y[8] = angularMomentum[2];
    y[9] = orientation.getScalar();
    y[10] = orientation.getVector()[0];
    y[11] = orientation.getVector()[1];
    y[12] = orientation.getVector()[2];

    return y;
}

void RigidBody::array2state(const std::vector<double> &y)
{
    assert(y.size() == 13);

    momentum = Vector3D(y[0], y[1], y[2]);
    position = Vector3D(y[3], y[4], y[5]);

    angularMomentum = Vector3D(y[6], y[7], y[8]);
    orientation = Quaternion(y[9], y[10], y[11], y[12]);
    orientation.normalise();

    computeDerivedStates();
}

void RigidBody::revertState()
{
    array2state(oldstates);
}

void RigidBody::applyBodyFieldForce(const Vector3D &force)
{
    this->force += orientation.rotate(force);
}

void RigidBody::applyBodyPointForce(const Vector3D &force, const Vector3D &position)
{
    this->force += orientation.rotate(force);
//this->torque += orientation.rotate(position ^ force);
    this->torque += (position ^ force);
}

void RigidBody::applyGlobalFieldForce(const Vector3D &force)
{
    this->force += force;
}

void RigidBody::applyGlobalPointForce(const Vector3D &force, const Vector3D &position)
{
    this->force += force;
//this->torque += (position ^ force);
    this->torque += (~orientation).rotate(position ^ force);
}

void RigidBody::applyBodyImpulse(const Vector3D &impulse, const Vector3D &position)
{
    momentum += orientation.rotate(impulse);
    angularMomentum += (position ^ impulse);

    computeDerivedStates();
}

void RigidBody::applyGlobalImpulse(const Vector3D &impulse, const Vector3D &position)
{
    momentum += impulse;
    angularMomentum += (~orientation).rotate(position ^ impulse);

    computeDerivedStates();
}

//accessors
const Vector3D& RigidBody::getForce() const
{
    return force;
}

const Vector3D& RigidBody::getMomentum() const
{
    return momentum;
}

const Vector3D& RigidBody::getPosition() const
{
    return position;
}

const Vector3D& RigidBody::getVelocity() const
{
    return velocity;
}

Vector3D RigidBody::getAcceleration() const
{
    return force / mass;
}

const Vector3D& RigidBody::getTorque() const
{
    return torque;
}

const Vector3D& RigidBody::getAngularMomentum() const
{
    return angularMomentum;
}

const Quaternion& RigidBody::getOrientation() const
{
    return orientation;
}

const Vector3D& RigidBody::getAngularVelocity() const
{
    return angularVelocity;
}

Vector3D RigidBody::getAngularAcceleration() const
{
    return inverseInertia * torque;
}

double RigidBody::getMass() const
{
    return mass;
}

double RigidBody::getInverseMass() const
{
    return inverseMass;
}

const Matrix3x3& RigidBody::getInertia() const
{
    return inertia;
}

const Matrix3x3& RigidBody::getInverseInertia() const
{
    return inverseInertia;
}

void RigidBody::getStates(Vector3D &position, Quaternion &orientation, Vector3D &momentum, Vector3D &angularMomentum) const
{
    position = this->position;
    orientation = this->orientation;
    momentum = this->momentum;
    angularMomentum = this->angularMomentum;
}

//mutators
void RigidBody::setForce(const Vector3D &force)
{
    this->force = force;
}

void RigidBody::setMomentum(const Vector3D &momentum)
{
    this->momentum = momentum;
    this->velocity = momentum / mass;
}

void RigidBody::setPosition(const Vector3D &position)
{
    this->position = position;
}

void RigidBody::setVelocity(const Vector3D &velocity)
{
    this->velocity = velocity;
    this->momentum = velocity * mass;
}

void RigidBody::setTorque(const Vector3D &torque)
{
    this->torque = torque;
}

void RigidBody::setAngularMomentum(const Vector3D &angularMomentum)
{
    this->angularMomentum = angularMomentum;
    this->angularVelocity = inverseInertia * angularMomentum;
}

void RigidBody::setOrientation(const Quaternion &orientation)
{
    this->orientation = orientation;
    this->orientation.normalise();
}

void RigidBody::setAngularVelocity(const Vector3D &angularVelocity)
{
    this->angularVelocity = angularVelocity;
    this->angularMomentum = inertia * angularVelocity;
}

void RigidBody::setMass(double mass)
{
    this->mass = mass;
    this->inverseMass = 1.0 / mass;
    this->velocity = momentum / mass;
}

void RigidBody::setInertia(const Matrix3x3 &inertia)
{
    this->inertia = inertia;
    this->inverseInertia = inertia.inverse();
    this->angularVelocity = inverseInertia * angularMomentum;
}

void RigidBody::setStates(const Vector3D &position, const Quaternion &orientation, const Vector3D &momentum, const Vector3D &angularMomentum)
{
    this->position = position;
    this->orientation = orientation;
    this->momentum = momentum;
    this->angularMomentum = angularMomentum;

    computeDerivedStates();
}

