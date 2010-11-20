#ifndef ACCLAIM_DISPLAY_H
#define ACCLAIM_DISPLAY_H

#include <GL/glu.h>

#include "KinematicModel.h"
#include "AcclaimJoint.h"

namespace AcclaimDisplay
{
    void draw(const KinematicModel<AcclaimJoint> &model, const Vector3D &colour);
    void drawAxis();
}

#endif
