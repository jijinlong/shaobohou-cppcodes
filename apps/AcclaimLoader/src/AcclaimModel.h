#ifndef ACCLAIM_MODEL_H
#define ACCLAIM_MODEL_H

#include "KinematicModel.h"
#include "AcclaimJoint.h"
#include "AcclaimModelParam.h"
#include "AcclaimMotion.h"


class AcclaimModel : public KinematicModel<AcclaimJoint>
{
    public:
        AcclaimModelParam getPose() const;
        void setPose(const AcclaimModelParam &params);

        void move(const AcclaimMove &move);
};

#endif
