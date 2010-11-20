#ifndef QUAKE_CAMERA_H
#define QUAKE_CAMERA_H

#include <GL/glu.h>
#include "BasicCamera.h"

//allow standard quake style movement control therefor certain restriction is applied
//no rotation about z axis, latitude must stay in certain threshold
class QuakeCamera:public BasicCamera
{
    public:
        QuakeCamera();
        QuakeCamera(const Vector3D &pos, double lat, double lon, double runSpeed, double turnSpeed, double sensitivity);
        virtual ~QuakeCamera();

        virtual void modOrientation(double rotateX, double rotateY, double rotateZ);	//mod orientation with euler angle radians
        virtual void modOrientation(double changeX, double changeY);	//mod orientation via mousr cursor movement
        virtual void reset();					//reset to look down -ve z axis

    private:
        double lon, lat;
};

#endif
