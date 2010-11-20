#include "AcclaimModelParam.h"

#include <cassert>

using std::string;
using std::vector;


AcclaimJointParam::AcclaimJointParam()
{}

AcclaimJointParam::AcclaimJointParam(const string &name, int nparams)
: m_name(name), m_params(nparams)
{}

AcclaimJointParam::AcclaimJointParam(const string &name, const vector<double> &params)
: m_name(name), m_params(params)
{}

const string& AcclaimJointParam::name() const
{
    return m_name;
}

int AcclaimJointParam::nparams() const
{
    return m_params.size();
}

double& AcclaimJointParam::params(int i)
{
    assert(i >= 0);
    assert(i < nparams());

    return m_params[i];
}

const double& AcclaimJointParam::params(int i) const
{
    assert(i >= 0);
    assert(i < nparams());

    return m_params[i];
}

vector<double>& AcclaimJointParam::params()
{
    return m_params;
}

const vector<double>& AcclaimJointParam::params() const
{
    return m_params;
}


AcclaimModelParam::AcclaimModelParam()
{}

AcclaimModelParam::AcclaimModelParam(int njoints)
: m_joints(njoints)
{}

int AcclaimModelParam::nparams() const
{
    int n = 0;
    for(int i = 0; i < njoints(); i++)
        n += joints(i).nparams();

    return n;
}

int AcclaimModelParam::njoints() const
{
    return m_joints.size();
}

AcclaimJointParam& AcclaimModelParam::joints(int i)
{
    assert(i >= 0);
    assert(i < njoints());

    return m_joints[i];
}

const AcclaimJointParam& AcclaimModelParam::joints(int i) const
{
    assert(i >= 0);
    assert(i < njoints());

    return m_joints[i];
}

int AcclaimModelParam::add(const AcclaimJointParam &joint)
{
    m_joints.push_back(joint);

    return njoints()-1;
}

vector<double> AcclaimModelParam::state2params() const
{
    std::vector<double> params;
    for(int i = 0; i < njoints(); i++)
        params.insert(params.end(), joints(i).params().begin(), joints(i).params().end());

    return params;
}

void AcclaimModelParam::params2state(const vector<double> &params)
{
    assert(nparams() == static_cast<int>(params.size()));

    int count = 0;
    for(int i = 0; i < njoints(); i++)
        for(int j = 0; j < joints(i).nparams(); j++)
        {
            joints(i).params(j) = params[count];
            count++;
        }
}
