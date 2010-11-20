#include "AcclaimModel.h"

#include "ExponentialMap.h"

using std::vector;


AcclaimModelParam AcclaimModel::getPose() const
{
    AcclaimModelParam param;
    for(int i = 0; i < njoints(); i++)
        param.add(AcclaimJointParam(joints(i).name(), joints(i).state2params()));

    return param;
}

void AcclaimModel::setPose(const AcclaimModelParam &param)
{
    for(int i = 0; i < param.njoints(); i++)
    {
        int ind = name2joint(param.joints(i).name());

        assert(ind >= 0);
        joints(ind).params2state(param.joints(i).params());
    }
    update();
}

void AcclaimModel::move(const AcclaimMove &move)
{
    Vector3D Rxz;
    double diff_Ry = 0.0;
    Vector3D diff_Txz;
    double Ty = 0.0;

    // update all joints, except root which is handled differently
    for(int i = 0; i < move.njoints(); i++)
    {
        const vector<double> &joint_param = move.joints(i).params();
        if(move.joints(i).name() == "root")
        {
            assert(joint_param.size() == 6);
            Rxz = Vector3D(joint_param[0], 0.0, joint_param[1]);
            diff_Ry = joint_param[2];
            diff_Txz = Vector3D(joint_param[3], 0.0, joint_param[4]);
            Ty = joint_param[5];
        }
        else
        {
            int ind = name2joint(move.joints(i).name());

            assert(ind >= 0);
            joints(ind).params2state(move.joints(i).params());
        }
    }

    // compute current rotation angle about vertical axis
    Quaternion default_pose(1.0, Vector3D());
    Vector3D yaxis(0.0, 1.0, 0.0);
    double Ry = computeAlignment(default_pose, root().local_orientation(), yaxis);

    // update current orientation
    Quaternion xzRotation = exp(Rxz);
    Quaternion yRotation = Quaternion::makeFromAxisAngle(yaxis, Ry + diff_Ry);
    root().local_orientation() = yRotation*xzRotation;

    // update current position;
    root().local_position() = root().local_position() + yRotation.rotate(diff_Txz);
    root().local_position()[1] = Ty;

    update();
}
