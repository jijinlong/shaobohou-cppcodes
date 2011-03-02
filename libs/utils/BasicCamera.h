#ifndef BASIC_CAMERA_H
#define BASIC_CAMERA_H

#include "Quaternion.h"
#include "Vector3D.h"
#include "Movement.h"

class BasicCamera
{
    public:
        Movement move;

        BasicCamera();
        BasicCamera(const Vector3D &pos, double runSpeed, double turnSpeed, double sensitivity);
        virtual ~BasicCamera();

        void setPosition(const Vector3D &position);
        const Vector3D& getPosition() const;
        const Quaternion& getOrientation() const;

        double getRunSpeed() const;
        double getTurnSpeed() const;
        void setRunSpeed(double newRunSpeed);
        void setTurnSpeed(double newTurnSpeed);

        virtual void setPerspective();        //set camera perspective
        virtual Vector3D update(double deltaTime);
        virtual void modOrientation(double rotateX, double rotateY, double rotateZ) = 0;    //modify orientation with change ineuler angle representation
        virtual void modOrientation(double changeX, double changeY) = 0;                    //modify orientation via mouse movement
        virtual void reset() = 0;                 //reset attributes

    protected:
        Quaternion orientation, trueOrientation;
        Vector3D position;
        double runSpeed, turnSpeed;
        double sensitivity;

        const static Quaternion cameraOffset;
};

#endif
