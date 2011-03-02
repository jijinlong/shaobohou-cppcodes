#ifndef KINEMATIC_MODEL_H
#define KINEMATIC_MODEL_H

#include "KinematicJoint.h"

#include <map>
#include <vector>
#include <string>
#include <cassert>


template <typename Joint>
class KinematicModel
{
    public:
        KinematicModel();
        virtual ~KinematicModel();

        Joint& root();
        const Joint& root() const;

        Joint& joints(int i);
        const Joint& joints(int i) const;
        int njoints() const;

        int ndofs() const;

        int name2joint(const std::string &n) const;

        Joint& getParent(const Joint &joint);
        const Joint& getParent(const Joint &joint) const;

        int addJoint(int parent, const Joint &joint);

        virtual void update();

    private:
        // special joint denote the origin of coordinate system for the model, is the parent of the root joint
        // can only be accessed with index < 0, initialised with zero transformation, modify at your own risk
        Joint m_origin;
        std::vector<Joint> m_joints;

        // mapping from name to joint and param
        std::map<std::string, int> m_name2joint;


        // recursive update function on joints(ind)
        void update(int ind);
};



// definitions

template <typename Joint>
KinematicModel<Joint>::KinematicModel()
: m_origin("origin", -1, -1)
{}

template <typename Joint>
KinematicModel<Joint>::~KinematicModel()
{}

template <typename Joint>
Joint& KinematicModel<Joint>::root()
{
    assert(njoints() > 0);

    return joints(0);
}

template <typename Joint>
const Joint& KinematicModel<Joint>::root() const
{
    assert(njoints() > 0);

    return joints(0);
}

template <typename Joint>
Joint& KinematicModel<Joint>::joints(int i)
{
    assert(i < njoints());

    if(i >= 0)
        return m_joints[i];
    else
        return m_origin;
}

template <typename Joint>
const Joint& KinematicModel<Joint>::joints(int i) const
{
    assert(i < njoints());

    if(i >= 0)
        return m_joints[i];
    else
        return m_origin;
}

template <typename Joint>
int KinematicModel<Joint>::njoints() const
{
    return static_cast<int>(m_joints.size());
}

template <typename Joint>
int KinematicModel<Joint>::ndofs() const
{
    int count = 0;
    for(int i = 0; i < njoints(); i++)
        count += joints(0).ndofs();

    return count;
}

template <typename Joint>
int KinematicModel<Joint>::name2joint(const std::string &n) const
{
    std::map<std::string, int>::const_iterator it = m_name2joint.find(n);

    if(it != m_name2joint.end())
        return it->second;
    else
        return -1;
}

template <typename Joint>
Joint& KinematicModel<Joint>::getParent(const Joint &joint)
{
    assert(joint.index() >= 0);         // to avoid cycling must not allow getParent on joint with index < 0, i.e. the origin joint
    assert(joint.index() < njoints());

    return joints(joint.parent());
}

template <typename Joint>
const Joint& KinematicModel<Joint>::getParent(const Joint &joint) const
{
    assert(joint.index() >= 0);         // to avoid cycling must not allow getParent on joint with index < 0, i.e. the origin joint
    assert(joint.index() < njoints());

    return joints(joint.parent());
}

template <typename Joint>
int KinematicModel<Joint>::addJoint(int parent, const Joint &joint)
{
//     assert(parent >= 0);
    assert(parent < njoints());

    m_joints.push_back(joint);

    int child = njoints()-1;
    joints(parent).addChild(child);

    // add joint to name2joint
    std::map<std::string, int>::const_iterator it = m_name2joint.find(joint.name());
    assert(it == m_name2joint.end());
    m_name2joint[joint.name()] = child;

    return child;
}

template <typename Joint>
void KinematicModel<Joint>::update()
{
    // call recursive update on root joint
    update(0);
}

template <typename Joint>
void KinematicModel<Joint>::update(int ind)
{
    // only update joints[ind] if it is not the origin joint
    if(ind < 0)
        return;

    // update joints[ind] w.r.t. its parent, the root joint's parent is the origin joint
    joints(ind).update(getParent(joints(ind)));

    // update children
    for(int i = 0; i < joints(ind).nchildren(); i++)
        update(joints(ind).children(i));
}

#endif
