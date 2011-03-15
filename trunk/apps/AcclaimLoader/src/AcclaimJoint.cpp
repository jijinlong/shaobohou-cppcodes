#include "AcclaimJoint.h"

#include <cmath>
#include <cassert>

using std::vector;
using std::string;
using std::max;
using std::min;


AcclaimJoint::AcclaimJoint()
: KinematicJoint()
{}

AcclaimJoint::AcclaimJoint(const std::string &name, int index, int parent)
: KinematicJoint(name, index, parent)
{}

AcclaimJoint::AcclaimJoint(const std::string &name, int index, int parent, const vector<int> &dof_orders,
                           const Vector3D &local_position_offset, const Quaternion &local_orientation_offset)
: KinematicJoint(name, index, parent, local_position_offset, local_orientation_offset),
  m_dofs(7, 0.0), m_dof_orders(dof_orders), m_local_position_offset(local_position_offset), m_local_orientation_offset(local_orientation_offset)
{}

AcclaimJoint::AcclaimJoint(const std::string &name, int index, int parent, const vector<int> &dof_orders,
                           const Vector3D &local_position_offset, const Quaternion &local_orientation_offset,
                           const Vector3D &local_endeffector)
: KinematicJoint(name, index, parent, local_position_offset, local_orientation_offset, local_endeffector),
  m_dofs(7, 0.0), m_dof_orders(dof_orders), m_local_position_offset(local_position_offset), m_local_orientation_offset(local_orientation_offset)
{}

std::vector<double> AcclaimJoint::state2params() const
{
    std::vector<double> params;
    for(unsigned int i = 0; i < m_dof_orders.size(); i++)
        params.push_back(m_dofs[m_dof_orders[i]]);

    return params;
}

void AcclaimJoint::params2state(const std::vector<double> &params)
{
    assert(params.size() == m_dof_orders.size());

    for(unsigned int i = 0; i < params.size(); i++)
        m_dofs[m_dof_orders[i]] = params[i];

    local_orientation() = m_local_orientation_offset * Quaternion::makeFromEulerAngles(deg2rad(m_dofs[0]), deg2rad(m_dofs[1]), deg2rad(m_dofs[2]));
    local_position() = m_local_position_offset + Vector3D(m_dofs[3], m_dofs[4], m_dofs[5]);
}
