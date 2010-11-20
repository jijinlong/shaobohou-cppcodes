#ifndef ACCLAIM_MODEL_PARAM_H
#define ACCLAIM_MODEL_PARAM_H

#include "Vector3D.h"
#include "Quaternion.h"

#include <string>
#include <vector>


class AcclaimJointParam
{
    public:
        AcclaimJointParam();
        AcclaimJointParam(const std::string &name, int nparams);
        AcclaimJointParam(const std::string &name, const std::vector<double> &params);

        const std::string& name() const;

        int nparams() const;
        double& params(int i);
        const double& params(int i) const;

        std::vector<double>& params();
        const std::vector<double>& params() const;

    private:
        std::string m_name;
        std::vector<double> m_params;
};


class AcclaimModelParam
{
    public:
        AcclaimModelParam();
        AcclaimModelParam(int njoints);

        int nparams() const;

        int njoints() const;
        AcclaimJointParam& joints(int i);
        const AcclaimJointParam& joints(int i) const;

        int add(const AcclaimJointParam &joint);

        std::vector<double> state2params() const;
        void params2state(const std::vector<double> &params);

    private:
        std::vector<AcclaimJointParam> m_joints;
};

#endif
