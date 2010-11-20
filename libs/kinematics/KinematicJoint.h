#ifndef KINEMATIC_JOINT_H
#define KINEMATIC_JOINT_H

#include "Vector3D.h"
#include "Quaternion.h"

#include <vector>


class KinematicJoint
{
    public:
        KinematicJoint();
        KinematicJoint(const std::string &name, int index, int parent);
        KinematicJoint(const std::string &name, int index, int parent, const Vector3D &local_position, const Quaternion &local_orientation);
        KinematicJoint(const std::string &name, int index, int parent, const Vector3D &local_position, const Quaternion &local_orientation, const Vector3D &local_endeffector);

        const std::string& name() const;

        int index() const;
        int parent() const;
        int children(int i) const;
        int nchildren() const;
        void addChild(int child);

        // modifiers and accessors for local transformation
        Vector3D& local_position();
        Quaternion& local_orientation();
        const Vector3D& local_position() const;
        const Quaternion& local_orientation() const;

        // modifier and accessors for global transformation, (may make global and local transformations out of sync)
        Vector3D& global_position();
        Quaternion& global_orientation();
        const Vector3D& global_position() const;
        const Quaternion& global_orientation() const;

        // functions for transforming from local coordinate space to global coordinate space
        Vector3D local2global(const Vector3D &local_point) const;
        Vector3D global2local(const Vector3D &global_point) const;

        // modifiers and accessors for endeffector
        Vector3D& local_endeffector();
        const Vector3D& local_endeffector() const;
        const Vector3D& global_endeffector() const;
        Vector3D global_endeffector_displacement() const;

        void update(const KinematicJoint &parent);
        void update(const Vector3D &parent_position, const Quaternion &parent_orientation);

    private:
        std::string m_name;

        int m_index;
        int m_parent;
        std::vector<int> m_children;

        // local transformation are defined in the parent's coordinate system
        // local position is the start of the joint, not the end effector
        Vector3D m_local_position;
        Quaternion m_local_orientation;
        Vector3D m_global_position;
        Quaternion m_global_orientation;

        // end effector of the joint, normally (but not necessarily) where the children joints are connected
        Vector3D m_local_endeffector;
        Vector3D m_global_endeffector;
};

#endif
