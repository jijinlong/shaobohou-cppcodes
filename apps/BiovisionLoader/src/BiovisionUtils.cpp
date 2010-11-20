#include "BiovisionUtils.h"

#include <iostream>

using std::ostream;
using std::endl;


void printJointPositionsHeader(ostream &out, const BiovisionModel &model)
{
    for(int i = 0; i < model.njoints(); i++)
        out << i+1 << "  " << i*3+1 << "  " << (i+1)*3 << "  " << model.joints(i).name() << "  " << model.joints(i).parent()+1 << endl;
}


void printJointPositions(ostream &out, const BiovisionModel &model, const BiovisionAnimation &animation)
{
    BiovisionModel tempModel = model;

    for(int i = 0; i < animation.nframes(); i++)
    {
        setPose(tempModel, animation.frames(i));
        for(int j = 0; j < tempModel.njoints(); j++)
        {
            Vector3D pos = tempModel.joints(j).global_position();
            out << pos[0] << " " << pos[1] << " " << pos[2] << " ";
        }
        out << endl;
    }
}


void printJointAnglesHeader(ostream &out, const BiovisionModel &model)
{
    int count = 0;
    for(int i = 0; i < model.njoints(); i++)
    {
        out << i+1 << "  " << count+1 << "  " << count+model.joints(i).ndofs() << "  " << model.joints(i).name() << "  " << model.joints(i).parent()+1 << endl;
        count += model.joints(i).ndofs();
    }
}

void printJointAngles(ostream &out, const BiovisionModel &model, const BiovisionAnimation &animation)
{
    BiovisionModel tempModel = model;

    for(int i = 0; i < animation.nframes(); i++)
    {
        setPose(tempModel, animation.frames(i));
        for(int j = 0; j < tempModel.njoints(); j++)
            for(int k = 0; k < tempModel.joints(j).ndofs(); k++)
                out << tempModel.joints(j).dofs(k) << " ";
        out << endl;
    }
}

