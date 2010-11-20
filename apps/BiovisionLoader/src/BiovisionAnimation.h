#ifndef BIOVISION_ANIMATION_H
#define BIOVISION_ANIMATION_H

#include <vector>


class BiovisionAnimation
{
    public:
        BiovisionAnimation();

        int nframes() const;
        std::vector<double>& frames(int i);
        const std::vector<double>& frames(int i) const;

        void add(const std::vector<double> &pose);

    private:
        std::vector<std::vector<double> > m_frames;
};

#endif
