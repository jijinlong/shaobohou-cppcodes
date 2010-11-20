#include "AcclaimMotion.h"

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
using std::cout;
using std::endl;


AcclaimMotion::AcclaimMotion()
{}

AcclaimMotion::AcclaimMotion(const string &filename)
{
    load(filename);
}

int AcclaimMotion::nmoves() const
{
    return m_moves.size();
}

AcclaimMove& AcclaimMotion::moves(int i)
{
    assert(i >= 0);
    assert(i < nmoves());

    return m_moves[i];
}

const AcclaimMove& AcclaimMotion::moves(int i) const
{
    assert(i >= 0);
    assert(i < nmoves());

    return m_moves[i];
}

AcclaimMove& AcclaimMotion::default_move()
{
    return m_default_move;
}

const AcclaimMove& AcclaimMotion::default_move() const
{
    return m_default_move;
}

void AcclaimMotion::load(const string &filename)
{
    ifstream in(filename.c_str(), ios::in);
    if (in.fail())
    {
        cout << "Failed to load open move file " << filename << endl;
        return;
    }

    vector<string> lines;
    getlines(in, lines);
    in.close();


    // read the default move
    assert(lines.size() > 0);
    const string &first_line = lines.front();
    vector<string> tokens = tokenise(first_line);

    m_default_move = AcclaimMove();
    for(unsigned int i = 0; i < tokens.size(); i += 2)
    {
        assert((i+1) < tokens.size());
        m_default_move.add(AcclaimJointParam(tokens[i], atoi(tokens[i+1].c_str())));
        std::cout << m_default_move.joints(m_default_move.njoints()-1).name() << std::endl;
    }


    // read the moves
    for(unsigned int i = 1; i < lines.size(); i++)
    {
        vector<string> tokens = tokenise(lines[i]);

        if(tokens.size() == 0)
            continue;

        m_moves.push_back(default_move());

        vector<double> params(tokens.size());
        for(unsigned int j = 0; j < tokens.size(); j++)
            params[j] = atof(tokens[j].c_str());

        m_moves.back().params2state(params);
    }
}
