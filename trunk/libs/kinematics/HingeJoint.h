#ifndef HINGE_JOINT_PARAM
#define HINGE_JOINT_PARAM

#include "KinematicJoint.h"

#include <string>


class HingeJoint : public KinematicJoint
{
    public:
        HingeJoint();
        HingeJoint(const std::string &name, int index, int parent);
        HingeJoint(const std::string &name, int index, int parent, const Vector3D &local_position,
                   const Vector3D &axis, double init_angle, double angle_min, double angle_max,
                   bool isFixed, const Vector3D &local_endeffector);

        void setAngle(double new_angle);
        const double& angle() const;
        const Vector3D& axis() const;

         bool is_fixed() const;

    private:
        double m_angle;

        Vector3D m_axis;
        double m_init_angle;
        double m_angle_min;
        double m_angle_max;

        bool m_is_fixed;
};

#endif
