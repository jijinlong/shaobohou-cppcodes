#ifndef BIOVISION_JOINT_H
#define BIOVISION_JOINT_H

#include "KinematicJoint.h"

#include <string>
#include <vector>

class BiovisionJoint : public KinematicJoint
{
    public:
        BiovisionJoint();
        BiovisionJoint(const std::string &name, int index, int parent);
        BiovisionJoint(const std::string &name, int index, int parent, const std::vector<int> &dof_orders,
                       const Vector3D &local_position_offset, const Quaternion &local_orientation_offset);

        int ndofs() const;
        const double& dofs(const int i) const;

        std::vector<double> state2params() const;
        void params2state(const std::vector<double> &params);

    private:
        std::vector<double> m_dofs;
        std::vector<int> m_dof_orders;

        Vector3D m_local_position_offset;
        Quaternion m_local_orientation_offset;
};

#endif
