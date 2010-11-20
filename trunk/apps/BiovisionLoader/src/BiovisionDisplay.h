#ifndef ACCLAIM_DISPLAY_H
#define ACCLAIM_DISPLAY_H

#include <GL/glu.h>

#include "KinematicModel.h"
#include "BiovisionJoint.h"

namespace BiovisionDisplay
{
    void draw(const KinematicModel<BiovisionJoint> &model, const Vector3D &colour, bool drawAxis);
}

#endif
