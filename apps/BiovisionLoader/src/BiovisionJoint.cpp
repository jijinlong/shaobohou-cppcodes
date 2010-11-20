#include "BiovisionJoint.h"

#include "special.h"

#include <cmath>
#include <cassert>

using std::vector;
using std::string;
using std::max;
using std::min;


BiovisionJoint::BiovisionJoint()
: KinematicJoint()
{}

BiovisionJoint::BiovisionJoint(const std::string &name, int index, int parent)
: KinematicJoint(name, index, parent)
{}

BiovisionJoint::BiovisionJoint(const std::string &name, int index, int parent, const vector<int> &dof_orders,
                               const Vector3D &local_position_offset, const Quaternion &local_orientation_offset)
: KinematicJoint(name, index, parent, local_position_offset, local_orientation_offset),
  m_dofs(7, 0.0), m_dof_orders(dof_orders), m_local_position_offset(local_position_offset), m_local_orientation_offset(local_orientation_offset)
{}

int BiovisionJoint::ndofs() const
{
    return m_dof_orders.size();
}

const double& BiovisionJoint::dofs(const int i) const
{
    assert(i >= 0);
    assert(i < ndofs());

    return m_dofs[m_dof_orders[i]];
}

std::vector<double> BiovisionJoint::state2params() const
{
    std::vector<double> params;
    for(unsigned int i = 0; i < m_dof_orders.size(); i++)
        params.push_back(m_dofs[m_dof_orders[i]]);

    return params;
}

void BiovisionJoint::params2state(const std::vector<double> &params)
{
    assert(params.size() == m_dof_orders.size());

    // update dofs
    for(unsigned int i = 0; i < params.size(); i++)
        m_dofs[m_dof_orders[i]] = params[i];

    // compute rotation
    Quaternion rotation;
    for(int i = 0; i < ndofs(); i++)
        if(m_dof_orders[i] == 0)
            rotation = rotation * Quaternion::makeFromAxisAngle(Vector3D(1.0, 0.0, 0.0), deg2rad(m_dofs[0]));
        else if(m_dof_orders[i] == 1)
            rotation = rotation * Quaternion::makeFromAxisAngle(Vector3D(0.0, 1.0, 0.0), deg2rad(m_dofs[1]));
        else if(m_dof_orders[i] == 2)
            rotation = rotation * Quaternion::makeFromAxisAngle(Vector3D(0.0, 0.0, 1.0), deg2rad(m_dofs[2]));

    // compute translation
    Vector3D translation(m_dofs[3], m_dofs[4], m_dofs[5]);

    // update local transformation
    local_orientation() = m_local_orientation_offset * rotation;
    local_position() = m_local_position_offset + translation;
}
