#include "AcclaimReader.h"

#include "MiscUtils.h"

#include <iostream>
#include <sstream>
#include <cassert>
#include <cstdlib>

using std::vector;
using std::string;
using std::map;
using std::istream;
using std::ifstream;
using std::stringstream;
using std::ios;
using std::cout;
using std::endl;


AcclaimBone::AcclaimBone()
: id(-1), length(-1.0), parent(-1)
{}

AcclaimBone::AcclaimBone(const string &bonename)
: name(bonename), id(-1), length(-1.0), parent(-1)
{}


AcclaimReader::AcclaimReader()
{}

AcclaimReader::AcclaimReader(const string &filename)
{
    dof2index["rx"] = 0;
    dof2index["ry"] = 1;
    dof2index["rz"] = 2;
    dof2index["tx"] = 3;
    dof2index["ty"] = 4;
    dof2index["tz"] = 5;
    dof2index["l"] = 6;


    ifstream in(filename.c_str(), ios::in);
    if (in.fail())
    {
        cout << "Failed to load open ASF file " << filename << endl;
        return;
    }

    vector<string> lines;
    getlines(in, lines);
    in.close();

    parseSkeleton(lines);

    computeLocalParameters();

    buildModel();

    m_model.update();
}

void AcclaimReader::parseSkeleton(const vector<string> &lines)
{
    for(unsigned int i = 0; i < lines.size(); i++)
    {
        vector<string> tokens = tokenise(lines[i]);

        if(tokens.size() == 0)
            continue;

        cout << lines[i] << endl;

        string keyword = lowercase(tokens.front());
        if(keyword == ":version")          //read file version
        {
            assert(tokens.size() > 0);

            version = tokens[1];
            if(version != "1.10")
            {
                cout << "Only version 1.10 is supported." << endl;
                exit(1);
            }
        }
        else if(keyword == ":name")        //read skeleton name
        {
            assert(tokens.size() > 0);

            name = tokens[1];
        }
        else if(keyword == ":units")       //read units
        {
            i = parseUnits(lines, i);
        }
        else if(keyword == ":documentation")
        {
            for(unsigned int j = i+1; j < lines.size(); j++)
            {
                vector<string> tokens = tokenise(lines[j]);

                if(tokens.size() == 0)
                    continue;

                if(tokens.front()[0] == ':')
                {
                    i = j - 1;
                    break;
                }
            }
        }
        else if(keyword == ":root")        //read root
        {
            i = parseRoot(lines, i);
        }
        else if(keyword == ":bonedata")    //read bonedata
        {
            i = parseBoneData(lines, i);
        }
        else if(keyword == ":hierarchy")   //read hierarchy
        {
            i = parseHierarchy(lines, i);
        }
    }
}

int AcclaimReader::parseUnits(const vector<string> &lines, int start)
{
    vector<string> tokens = tokenise(lines[start]);

    assert(tokens.size() > 0);
    assert(tokens.front() == ":units");

    unsigned int i = 0;
    for(i = start+1; i < lines.size(); i++)
    {
        tokens = tokenise(lines[i]);

        if(tokens.size() == 0)
            continue;

        string keyword = lowercase(tokens.front());
        if(keyword == "mass")       //read mass unit
        {
            assert(tokens.size() > 1);
            mass = atof(tokens[1].c_str());
        }
        else if(keyword == "length") //read length unit
        {
            assert(tokens.size() > 1);
            length = atof(tokens[1].c_str());
        }
        else if(keyword == "angle") //read angle unit
        {
            assert(tokens.size() > 1);
            angle = tokens[1];
        }
        else if(keyword[0] == ':')
            break;
    }

    return i-1;
}

int AcclaimReader::parseRoot(const vector<string> &lines, int start)
{
    vector<string> tokens = tokenise(lines[start]);

    assert(tokens.size() > 0);
    assert(tokens.front() == ":root");

    bones.push_back(AcclaimBone("root"));    // root bone
    map<string, int>::const_iterator it = bone2index.find(bones.back().name);
    assert(it == bone2index.end());
    bone2index[bones.back().name] = bones.size()-1;

    unsigned int i = 0;
    for(i = start+1; i < lines.size(); i++)
    {
        tokens = tokenise(lines[i]);

        if(tokens.size() == 0)
            continue;

        string keyword = lowercase(tokens.front());
        if(keyword == "axis")              //read root axis order
        {
            assert(tokens.size() > 1);

            if(lowercase(tokens[1]) != "xyz")
            {
                cout << "Error while reading root << " << ", only xyz rotation order supported, " << tokens[1] << " was found." << endl;
                exit(1);
            }
        }
        else if(keyword == "order")        //read transformation order
        {
            for(unsigned int j = 1; j < tokens.size(); j++)
            {
                string tok = lowercase(tokens[j]);
                map<string, int>::const_iterator it = dof2index.find(tok);

                assert(it != dof2index.end());
                bones.front().orders.push_back(it->second);
            }
        }
        else if(keyword == "position")     //read root position
        {
            assert(tokens.size() > 3);

            root_position[0] = atof(tokens[1].c_str());
            root_position[1] = atof(tokens[2].c_str());
            root_position[2] = atof(tokens[3].c_str());
        }
        else if(keyword == "orientation")  //read root orientation
        {
            assert(tokens.size() > 3);

            root_angles[0] = atof(tokens[1].c_str());
            root_angles[1] = atof(tokens[2].c_str());
            root_angles[2] = atof(tokens[3].c_str());
        }
        else
            break;
    }

    return i-1;
}

int AcclaimReader::parseBoneData(const vector<string> &lines, int start)
{
    vector<string> tokens = tokenise(lines[start]);

    assert(tokens.size() > 0);
    assert(tokens.front() == ":bonedata");

    bool begun = false;
    unsigned int i = 0;
    for(i = start+1; i < lines.size(); i++)
    {
        tokens = tokenise(lines[i]);

        if(tokens.size() == 0)
            continue;

        string keyword = lowercase(tokens.front());
        if(keyword == "begin")         //read begin
        {
            assert(!begun);

            begun = true;
            bones.push_back(AcclaimBone());
        }
        else if(keyword == "id")       //read bone id
        {
            assert(tokens.size() > 0);
            bones.back().id = atoi(tokens[1].c_str());
        }
        else if(keyword == "name")     //read bone name
        {
            assert(tokens.size() > 0);
            bones.back().name = tokens[1];

            map<string, int>::const_iterator it = bone2index.find(keyword);
            assert(it == bone2index.end());

            bone2index[bones.back().name] = bones.size()-1;
        }
        else if(keyword == "direction") //read bone direction
        {
            assert(tokens.size() > 3);
            bones.back().global_direction[0] = atof(tokens[1].c_str());
            bones.back().global_direction[1] = atof(tokens[2].c_str());
            bones.back().global_direction[2] = atof(tokens[3].c_str());
        }
        else if(keyword == "length")   //read bone length
        {
            assert(tokens.size() > 0);
            bones.back().length = atof(tokens[1].c_str());
        }
        else if(keyword == "axis")     //read bone axis
        {
            assert(tokens.size() > 4);
            bones.back().axis[0] = atof(tokens[1].c_str());
            bones.back().axis[1] = atof(tokens[2].c_str());
            bones.back().axis[2] = atof(tokens[3].c_str());

            string val = lowercase(tokens[4]);
            if(val != "xyz")
            {
                cout << "Error while reading bone " << bones.back().name << ", only xyz rotation order supported, " << tokens[1] << " was found." << endl;
                exit(1);
            }
        }
        else if(keyword == "dof")     //read bone dof
        {
            for(unsigned int j = 1; j < tokens.size(); j++)
            {
                string tok = lowercase(tokens[j]);
                map<string, int>::const_iterator it = dof2index.find(tok);

                assert(it != dof2index.end());
                bones.back().orders.push_back(it->second);
            }
        }
        else if(keyword == "end")      //read end
        {
            assert(begun);

            begun = false;
        }
        else if(keyword[0] == ':')
            break;
    }

    return i-1;
}

int AcclaimReader::parseHierarchy(const vector<string> &lines, int start)
{
    vector<string> tokens = tokenise(lines[start]);

    assert(tokens.size() > 0);
    assert(tokens.front() == ":hierarchy");

    bool begun = false;
    unsigned int i = 0;
    for(i = start+1; i < lines.size(); i++)
    {
        tokens = tokenise(lines[i]);

        if(tokens.size() == 0)
            continue;

        string keyword = lowercase(tokens.front());
        if(keyword == "begin")
        {
            assert(!begun);
            begun = true;
        }
        else if(keyword == "end")
        {
            assert(begun);
            begun = false;
        }
        else if(keyword[0] == ':')
        {
            break;
        }
        else
        {
            map<string, int>::const_iterator it = bone2index.find(keyword);
            assert(it != bone2index.end());

            int parent = it->second;
            for(unsigned int j = 1; j < tokens.size(); j++)
            {
                it = bone2index.find(lowercase(tokens[j]));
                assert(it != bone2index.end());

                int child = it->second;
                bones[parent].children.push_back(child);
                bones[child].parent = parent;
            }
        }
    }

    return i-1;
}

void AcclaimReader::computeLocalParameters()
{
    root_orientation = Quaternion::makeFromEulerAngles(root_angles[0], root_angles[1], root_angles[2]);

    bones.front().length = 0.0;
    bones.front().global_orientation = root_orientation;
    bones.front().local_orientation_offset = root_orientation;

    computeLocalOffset(0);
}

void AcclaimReader::buildModel()
{
    m_model = AcclaimModel();

    for(unsigned int i = 0; i < bones.size(); i++)
    {
        Vector3D local_position;
        Quaternion local_orientation_offset;
        if(bones[i].parent >= 0)
        {
            local_position = bones[bones[i].parent].local_direction*bones[bones[i].parent].length;
            local_orientation_offset = bones[i].local_orientation_offset;
        }
        Vector3D local_endeffector = bones[i].local_direction*bones[i].length;

        m_model.addJoint(bones[i].parent, AcclaimJoint(bones[i].name, i, bones[i].parent, bones[i].orders, local_position, local_orientation_offset, local_endeffector));
    }
}

const AcclaimModel& AcclaimReader::model() const
{
    return m_model;
}

void AcclaimReader::computeLocalOffset(int ind)
{
    if(bones[ind].parent >= 0)
    {
        bones[ind].global_orientation = Quaternion::makeFromEulerAngles(deg2rad(bones[ind].axis[0]), deg2rad(bones[ind].axis[1]), deg2rad(bones[ind].axis[2]));
        bones[ind].local_orientation_offset = (~(bones[bones[ind].parent].global_orientation)) * bones[ind].global_orientation;
        bones[ind].local_direction = (~bones[ind].global_orientation).rotate(bones[ind].global_direction);  //rotate bone direction to local space
    }

    for(unsigned int i = 0; i < bones[ind].children.size(); i++)
        computeLocalOffset(bones[ind].children[i]);
}
