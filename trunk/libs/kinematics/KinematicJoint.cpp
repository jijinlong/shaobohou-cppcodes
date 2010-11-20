#include "KinematicJoint.h"

#include <cassert>
#include <cmath>

using std::string;
using std::vector;
using std::max;
using std::min;


KinematicJoint::KinematicJoint()
: m_index(-1), m_parent(-1)
{}

KinematicJoint::KinematicJoint(const string &name, int index, int parent)
: m_name(name), m_index(index), m_parent(parent)
{}

KinematicJoint::KinematicJoint(const string &name, int index, int parent, const Vector3D &local_position, const Quaternion &local_orientation)
: m_name(name), m_index(index), m_parent(parent), m_local_position(local_position), m_local_orientation(local_orientation)
{
    update(Vector3D(), Quaternion());
}

KinematicJoint::KinematicJoint(const string &name, int index, int parent, const Vector3D &local_position, const Quaternion &local_orientation, const Vector3D &local_endeffector)
: m_name(name), m_index(index), m_parent(parent), m_local_position(local_position), m_local_orientation(local_orientation),
  m_local_endeffector(local_endeffector)
{
    update(Vector3D(), Quaternion());
}

const string& KinematicJoint::name() const
{
    return m_name;
}

int KinematicJoint::index() const
{
    return m_index;
}

int KinematicJoint::parent() const
{
    return m_parent;
}

int KinematicJoint::children(int i) const
{
    return m_children.at(i);
}

int KinematicJoint::nchildren() const
{
    return m_children.size();
}

void KinematicJoint::addChild(int child)
{
    m_children.push_back(child);
}

Vector3D& KinematicJoint::local_position()
{
    return m_local_position;
}

Quaternion& KinematicJoint::local_orientation()
{
    return m_local_orientation;
}

const Vector3D& KinematicJoint::local_position() const
{
    return m_local_position;
}

const Quaternion& KinematicJoint::local_orientation() const
{
    return m_local_orientation;
}

Vector3D& KinematicJoint::global_position()
{
    return m_global_position;
}

Quaternion& KinematicJoint::global_orientation()
{
    return m_global_orientation;
}

const Vector3D& KinematicJoint::global_position() const
{
    return m_global_position;
}

const Quaternion& KinematicJoint::global_orientation() const
{
    return m_global_orientation;
}

Vector3D KinematicJoint::local2global(const Vector3D &local_point) const
{
    return global_position() + global_orientation().rotate(local_point);
}

Vector3D KinematicJoint::global2local(const Vector3D &global_point) const
{
    return (~global_orientation()).rotate(global_point - global_position());
}

Vector3D& KinematicJoint::local_endeffector()
{
    return m_local_endeffector;
}

const Vector3D& KinematicJoint::local_endeffector() const
{
    return m_local_endeffector;
}

const Vector3D& KinematicJoint::global_endeffector() const
{
    return m_global_endeffector;
}

Vector3D KinematicJoint::global_endeffector_displacement() const
{
    return global_endeffector()-global_position();
}

void KinematicJoint::update(const KinematicJoint &parent)
{
    update(parent.global_position(), parent.global_orientation());
}

void KinematicJoint::update(const Vector3D &parent_position, const Quaternion &parent_orientation)
{
    m_global_position = parent_position + parent_orientation.rotate(local_position());
    m_global_orientation = parent_orientation * local_orientation();
    m_global_orientation.normalise();

    // update endeffector
    m_global_endeffector = global_position() + global_orientation().rotate(local_endeffector());
}
