#ifndef SPACE_FLIGHT_BODY_H
#define SPACE_FLIGHT_BODY_H

#include "RigidBody.h"
#include "GraphicsMaths.h"

class SpaceFlightBody: public RigidBody
{
    public:
        SpaceFlightBody();
        SpaceFlightBody(double mass, const Matrix3x3 &inertia, double linearResistance, double angularResistance);
        virtual ~SpaceFlightBody();

        //function to compute total force and total torque at any point in time given the ys, need to be implmented by subclass
        virtual void computeForceAndTorque(double time);

    protected:
        double    linearResistance,
        angularResistance;

    private:
        void init();
};

#endif
