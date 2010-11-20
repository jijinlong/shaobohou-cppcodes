#ifndef QUAT_CAMERA_H
#define QUAT_CAMERA_H

#include "BasicCamera.h"

class QuatCamera:public BasicCamera
{
    public:
        QuatCamera();
        QuatCamera(const Vector3D &pos, const Quaternion &orientation, double runSpeed, double turnSpeed, double sensitivity);
        virtual ~QuatCamera();

        void setOrientation(const Quaternion &orientation);

        virtual void modOrientation(double rotateX, double rotateY, double rotateZ);	//mod orientation via euler angles in radians
        virtual void modOrientation(double changeX, double changeY);	//mod orientation via mouse cursor movement
        virtual void reset();					//reset to look down -ve z axis
};

#endif
