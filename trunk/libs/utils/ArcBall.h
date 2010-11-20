#ifndef ARCBALL_H
#define ARCBALL_H

#include "Quaternion.h"

class ArcBall
{
    public:
        ArcBall();
        ArcBall(unsigned int windowWidth, unsigned int windowHeight);

        void setWindowSize(unsigned int windowWidth, unsigned int windowHeight);
        void setSpherePosition(unsigned int xPos, unsigned int yPos);
        Vector3D mapToSphere(unsigned int xPos, unsigned int yPos) const;
        Quaternion drag(unsigned int xPos, unsigned int yPos);

    protected:
        unsigned int windowWidth, windowHeight;
        Vector3D spherePosition;
};

#endif

