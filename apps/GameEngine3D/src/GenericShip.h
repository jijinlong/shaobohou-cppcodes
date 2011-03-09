#ifndef GENERIC_SHIP_H
#define GENERIC_SHIP_H

#include "RigidBody.h"
#include "GraphicsMaths.h"

class Thruster
{
    public:
        Thruster();
        Thruster(const Vector3D &position, const Vector3D &direction);
        virtual ~Thruster();

        const Vector3D& getPosition() const;
        const Vector3D& getDirection() const;

    protected:
        Vector3D position;
        Vector3D direction;
};

class GenericShip: public RigidBody
{
    public:
        Vector3D    rotation;            //indicate desired rotation of the ship, mapped to user control, each element indicate rotate about that axis
        Vector3D    movement;            //indicate desired movement of the ship, mapped to user control, each element indicate movement along that axis
        double    	turnPower,
        forwardPower;

        GenericShip();
        GenericShip(double mass, const Matrix3x3 &inertia, double linearResistance, double angularResistance);
        virtual ~GenericShip();

        virtual void computeForceAndTorque(double time);

        void fireThruster(const Thruster &thruster, double power);
        double maxSpeed(double power);
        double forwardPowerRequiredForSpeed(double speed);

    protected:
        Vector3D	direction;         //intended direction
        double    	linearResistance,
        angularResistance;

    private:
        Thruster xPos, xNeg;  //for turn thrusters, x/y/z indicate which axis they try to rotate
        Thruster yPos, yNeg;  //Pos/Neg, indicate which part of a axis they lie on (not necessarily the rotation axis)
        Thruster zPos, zNeg;  //direction of a pair of thruster should be that if the Pos one is fired forward and Neg one is fired backward then the rotation caused should be counter-clockwise
        Thruster forward;

        void init();
};

#endif
