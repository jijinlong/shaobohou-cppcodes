#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include "Integrator.h"
#include "Quaternion.h"
#include "Matrix3x3.h"

//linear and angular momentum is better as they can be conserved

class RigidBody: public Integratable
{
    public:
        RigidBody();
        RigidBody(double mass, const Matrix3x3 &inertia);
        virtual ~RigidBody();

        virtual void computeForceAndTorque(double time) = 0;
        virtual void step(double time, double deltaTime);  //integrate the motion from time to time + deltaTime

        virtual std::vector<double> derive(double t, const std::vector<double> &y) const;
        virtual void revertState();

        //both function apply forces in body coordinate
        void applyBodyFieldForce(const Vector3D &force);
        void applyBodyPointForce(const Vector3D &force, const Vector3D &position);

        //both function apply forces in global coordinate
        void applyGlobalFieldForce(const Vector3D &force);
        void applyGlobalPointForce(const Vector3D &force, const Vector3D &position);

        //apply impulse
        void applyBodyImpulse(const Vector3D &impulse, const Vector3D &position);
        void applyGlobalImpulse(const Vector3D &impulse, const Vector3D &position);

        //accessors
        const Vector3D& getForce() const;
        const Vector3D& getMomentum() const;
        const Vector3D& getPosition() const;
        const Vector3D& getVelocity() const;
        Vector3D getAcceleration() const;

        const Vector3D& getTorque() const;
        const Vector3D& getAngularMomentum() const;
        const Quaternion& getOrientation() const;
        const Vector3D& getAngularVelocity() const;
        Vector3D getAngularAcceleration() const;

        double getMass() const;
        double getInverseMass() const;
        const Matrix3x3& getInertia() const;
        const Matrix3x3& getInverseInertia() const;

        //get all state (position, momentum, orientation, angular momentum)
        void getStates(Vector3D &position, Quaternion &orientation, Vector3D &momentum, Vector3D &angularMomentum) const;

        //mutators
        void setForce(const Vector3D &force);
        void setMomentum(const Vector3D &momentum);
        void setPosition(const Vector3D &position);
        void setVelocity(const Vector3D &velocity);

        void setTorque(const Vector3D &torque);
        void setAngularMomentum(const Vector3D &angularMomentum);
        void setOrientation(const Quaternion &orientation);
        void setAngularVelocity(const Vector3D &angularVelocity);

        void setMass(double mass);
        void setInertia(const Matrix3x3 &inertia);

        //set all state (position, momentum, orientation, angular momentum)
        void setStates(const Vector3D &position, const Quaternion &orientation, const Vector3D &momentum, const Vector3D &angularMomentum);

    protected:
        Integrator integrator;       //integrator
        std::vector<double> oldstates;

        //linear motion, defined relative to frame of reference coordinate system
        Vector3D    force,            //total force acting on body centre of gravity, do not induce angular motion
        momentum,			//use momentum here to be consistent with angular motion
        position;         //centre of gravity, probably the same as design centre of the model
        Vector3D    velocity;			//derived from momentum/mass

        //angular motion, defined in body coordinate, less likely to cause numerical error
        Vector3D    torque,           //total torque caused by force acting on body in the form r x f (PRESERVE ORDER OF VECTOR CROSS PRODUCT). F, force Vector; r, Vector from centre of gravity to point of application of force
        angularMomentum;  //angular momentumm is more stable and easier to make calculation with
        Quaternion  orientation;
        Vector3D    angularVelocity;	//direction of the axis of rotation and magnitude is magnitude of velocity,  = angularMomentum / inertia essentially

        //these are most likely fixed quantity in the simulation sense
        double      mass,             //must becareful about division by 0 in calculation involving mass, negative will used to indicate infinite mass. Infinite mass means that linear motion and angular motion (infinite inertia) cannot be changed.
        inverseMass;
        Matrix3x3   inertia,          //inertia tensor, in body coordinate about centre of gravity in design coordinate axis
        inverseInertia;   //inverseInertia cannot be set explicitly, must be calculate from inertia tensor

        void computeDerivedStates();  //compute velocity and angular velocity from other states

        virtual std::vector<double> state2array() const;
        virtual void array2state(const std::vector<double> &y);

    private:
        void init();
};

#endif
