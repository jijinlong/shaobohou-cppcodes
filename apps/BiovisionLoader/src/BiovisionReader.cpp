#include "BiovisionReader.h"

#include "MiscUtils.h"
#include "special.h"

#include <iostream>
#include <fstream>
#include <cstdlib>

using std::string;
using std::vector;
using std::ifstream;
using std::ios;
using std::cout;
using std::endl;


BiovisionBone::BiovisionBone()
{
}

BiovisionBone::BiovisionBone(const std::string &bonename)
: name(bonename)
{
}

BiovisionReader::BiovisionReader()
: total_nchannels(0)
{
}

BiovisionReader::BiovisionReader(const string &filename)
: total_nchannels(0)
{
    ifstream in(filename.c_str(), ios::in);
    if (in.fail())
    {
        cout << "Failed to load open BVH file " << filename << endl;
        return;
    }

    vector<string> lines;
    getlines(in, lines);
    in.close();


    parse(lines);

    buildModel();

    m_model.update();
}

const BiovisionModel& BiovisionReader::model() const
{
    return m_model;
}

const BiovisionAnimation& BiovisionReader::animation() const
{
    return m_animation;
}

void BiovisionReader::parse(const vector<string> &lines)
{
    for(unsigned int i = 0; i < lines.size(); i++)
    {
        vector<string> tokens = tokenise(lines[i]);

        if(tokens.size() == 0)
            continue;

        cout<< lines[i] << endl;

        string keyword = lowercase(tokens.front());
        if(keyword == "hierarchy")    //read bonedata
        {
            i = parseHierarchy(lines, i);
        }
        else if(keyword == "motion")   //read hierarchy
        {
            i = parseMotion(lines, i);
        }
    }
}

int BiovisionReader::parseHierarchy(const vector<string> &lines, int start)
{
    int nbrackets = 0;
    bool readAnything = false;
    int curr_parent = -1;

    unsigned int i = 0;
    for(i = start+1; i < lines.size(); i++)
    {
        vector<string> tokens = tokenise(lines[i]);

        if(tokens.size() == 0)
            continue;

        string keyword = lowercase(tokens.front());
        if(keyword == "{")
        {
            nbrackets++;
            curr_parent = static_cast<int>(bones.size())-1;

            readAnything = true;
        }
        else if(keyword == "}")
        {
            assert(nbrackets > 0);
            assert(curr_parent >= 0);

            nbrackets--;
            curr_parent = bones[curr_parent].parent;
        }
        else if((keyword == "root") || (keyword == "joint") || (keyword == "end"))
        {
            assert(tokens.size() > 1);

            if(keyword == "end")
                bones.push_back(bones[curr_parent].name+"End");
            else
                bones.push_back(BiovisionBone(tokens[1]));

            bones.back().parent = curr_parent;
            if(curr_parent >= 0)
                bones[curr_parent].children.push_back(static_cast<int>(bones.size())-1);

        }
        else if(keyword == "offset")
        {
            assert(tokens.size() > 3);

            bones.back().offset[0] = atof(tokens[1].c_str());
            bones.back().offset[1] = atof(tokens[2].c_str());
            bones.back().offset[2] = atof(tokens[3].c_str());
        }
        else if(keyword == "channels")
        {
            assert(tokens.size() > 1);

            int nchannels = atoi(tokens[1].c_str());
            total_nchannels += nchannels;

            assert(static_cast<int>(tokens.size()) > 1+nchannels);
            for(int j = 2; j < 2+nchannels; j++)
            {
                const string tok = lowercase(tokens[j]);
                if(tok == "xposition")
                    bones.back().orders.push_back(3);
                else if(tok == "yposition")
                    bones.back().orders.push_back(4);
                else if(tok == "zposition")
                    bones.back().orders.push_back(5);
                else if(tok == "xrotation")
                    bones.back().orders.push_back(0);
                else if(tok == "yrotation")
                    bones.back().orders.push_back(1);
                else if(tok == "zrotation")
                    bones.back().orders.push_back(2);
                else
                    assert(false);
            }
        }

        if(readAnything)
            if(nbrackets == 0)
                break;
    }

    return i;
}

int BiovisionReader::parseMotion(const vector<string> &lines, int start)
{
    int nframes = 0;
    double frame_time = 0.0;

    unsigned int i = 0;
    for(i = start+1; i < lines.size(); i++)
    {
        vector<string> tokens = tokenise(lines[i]);

        if(tokens.size() == 0)
            continue;

        string keyword = lowercase(tokens.front());
        if(keyword == "frames:")
        {
            assert(tokens.size() > 1);

            nframes = atoi(tokens[1].c_str());
        }
        else if(keyword == "frame")
        {
            assert(tokens.size() > 2);

            frame_time = atof(tokens[2].c_str());
        }
        else
        {
            assert(static_cast<int>(tokens.size()) >= total_nchannels);

            std::vector<double> pose;
            for(unsigned int j = 0; j < tokens.size(); j++)
                pose.push_back(atof(tokens[j].c_str()));

            assert(static_cast<int>(pose.size()) == total_nchannels);
            m_animation.add(pose);
        }
    }

    assert(m_animation.nframes() == nframes);

    return i;
}

void BiovisionReader::buildModel()
{
    m_model = BiovisionModel();

    for(unsigned int i = 0; i < bones.size(); i++)
    {
        Vector3D local_position;
        if(bones[i].parent >= 0)    // ignore offset for root
            local_position = bones[i].offset;

        m_model.addJoint(bones[i].parent, BiovisionJoint(bones[i].name, i, bones[i].parent, bones[i].orders, local_position, Quaternion()));
    }
}

void setPose(BiovisionModel &model, const std::vector<double> &pose)
{
    int count = 0;
    for(int i = 0; i < model.njoints(); i++)
    {
        const int ndofs = model.joints(i).ndofs();
        std::vector<double> params(ndofs, 0.0);

        assert((count+ndofs) <= static_cast<int>(pose.size()));
        for(int j = 0; j < ndofs; j++)
            params[j] = pose[count+j];
        count += ndofs;

        model.joints(i).params2state(params);
    }
    model.update();
}
