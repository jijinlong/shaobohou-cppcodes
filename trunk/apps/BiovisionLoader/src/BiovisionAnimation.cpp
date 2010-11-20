#include "BiovisionAnimation.h"

#include <cassert>

using std::vector;


BiovisionAnimation::BiovisionAnimation()
{
}

int BiovisionAnimation::nframes() const
{
    return m_frames.size();
}

vector<double>& BiovisionAnimation::frames(int i)
{
    assert(i >= 0);
    assert(i < nframes());

    return m_frames[i];
}

const vector<double>& BiovisionAnimation::frames(int i) const
{
    assert(i >= 0);
    assert(i < nframes());

    return m_frames[i];
}

void BiovisionAnimation::add(const vector<double> &pose)
{
    if(nframes() > 0)
        assert(pose.size() == frames(0).size());
    m_frames.push_back(pose);
}
