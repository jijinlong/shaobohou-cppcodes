#include "HingeJoint.h"

#include <cmath>

using std::string;
using std::max;
using std::min;

const double PI = 3.1415926;
const double TWO_PI = PI*2;


HingeJoint::HingeJoint()
: KinematicJoint(),
  m_angle(0.0), m_init_angle(0.0), m_angle_min(0.0), m_angle_max(0.0), m_is_fixed(false)
{}

HingeJoint::HingeJoint(const string &name, int index, int parent)
: KinematicJoint(name, index, parent),
  m_angle(0.0), m_init_angle(0.0), m_angle_min(0.0), m_angle_max(0.0), m_is_fixed(false)
{}

HingeJoint::HingeJoint(const string &name, int index, int parent, const Vector3D &local_position,
                       const Vector3D &axis, double init_angle, double angle_min, double angle_max,
                       bool isFixed, const Vector3D &local_endeffector)
: KinematicJoint(name, index, parent, local_position, Quaternion::makeFromAxisAngle(axis, init_angle), local_endeffector),
  m_angle(init_angle), m_axis(axis), m_init_angle(init_angle), m_angle_min(angle_min), m_angle_max(angle_max),
  m_is_fixed(isFixed)
{
    while(m_angle_min > +PI) m_angle_min -= TWO_PI;
    while(m_angle_max > +PI) m_angle_max -= TWO_PI;
    while(m_angle_min < -PI) m_angle_min += TWO_PI;
    while(m_angle_max < -PI) m_angle_max += TWO_PI;

    setAngle(init_angle);
    m_init_angle = m_angle;
}

void HingeJoint::setAngle(double new_angle)
{
    if(new_angle >= 0.0)
    {
        m_angle = +fmod(+new_angle, TWO_PI);
        if(m_angle > +PI) m_angle -= TWO_PI;
    }
    else
    {
        m_angle = -fmod(-new_angle, TWO_PI);
        if(m_angle < -PI) m_angle += TWO_PI;
    }

    m_angle = min(m_angle_max, max(m_angle, m_angle_min));
    local_orientation() = Quaternion::makeFromAxisAngle(m_axis, m_angle);
}

const double& HingeJoint::angle() const
{
    return m_angle;
}

const Vector3D& HingeJoint::axis() const
{
    return m_axis;
}

bool HingeJoint::is_fixed() const
{
    return m_is_fixed;
}
