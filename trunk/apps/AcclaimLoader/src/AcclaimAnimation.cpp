#include "AcclaimAnimation.h"

#include "MiscUtils.h"

#include <sstream>
#include <fstream>
#include <iostream>
#include <cassert>
#include <cstdlib>

using std::vector;
using std::string;
using std::ios;
using std::ifstream;
using std::ofstream;
using std::stringstream;
using std::ios;
using std::cout;
using std::endl;


AcclaimAnimation::AcclaimAnimation()
{
}

AcclaimAnimation::AcclaimAnimation(const string &filename)
{
    load(filename);
}

int AcclaimAnimation::nframes() const
{
    return m_frames.size();
}

AcclaimModelParam& AcclaimAnimation::frames(int i)
{
    assert(i >= 0);
    assert(i < nframes());

    return m_frames[i];
}

const AcclaimModelParam& AcclaimAnimation::frames(int i) const
{
    assert(i >= 0);
    assert(i < nframes());

    return m_frames[i];
}

void AcclaimAnimation::load(const string &filename)
{
    ifstream in(filename.c_str(), ios::in);
    if (in.fail())
    {
        cout << "Failed to load open AMC file " << filename << endl;
        return;
    }


    // clear previous frames
    m_frames.clear();
    vector<string> lines;
    getlines(in, lines);
    in.close();

    // look for the start of the first frame
    unsigned int start = 0;
    for(unsigned int i = 0; i < lines.size(); i++)
    {
        vector<string> tokens = tokenise(lines[i]);

        if(tokens.size() == 0)
            continue;

        string keyword = lowercase(tokens.front());
        if(atoi(keyword.c_str()) == (nframes()+1))
        {
            start = i;
            break;
        }
    }

    // read all frames
    for(unsigned int i = start; i < lines.size(); i++)
	{
        vector<string> tokens = tokenise(lines[i]);

        if(tokens.size() == 0)
            continue;

        string keyword = lowercase(tokens.front());
		if(atoi(keyword.c_str()) == (nframes()+1))
        {
            m_frames.push_back(AcclaimModelParam());
        }
        else
        {
            vector<double> bone_dofs;
            for(unsigned int j = 1; j < tokens.size(); j++)
                bone_dofs.push_back(atof(tokens[j].c_str()));

            m_frames.back().add(AcclaimJointParam(keyword, bone_dofs));
        }
	}


    // check dofs loaded correctly
    for(int i = 1; i < nframes(); i++)
    {
        assert(frames(i).njoints() == frames(i-1).njoints());

        for(int j = 0; j < frames(i).njoints(); j++)
        {
            assert(frames(i).joints(j).name() == frames(i-1).joints(j).name());
            assert(frames(i).joints(j).nparams() == frames(i-1).joints(j).nparams());
        }
    }
}
