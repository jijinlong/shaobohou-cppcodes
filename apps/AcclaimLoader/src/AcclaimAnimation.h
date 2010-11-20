#ifndef ACCLAIM_ANIMATION_H
#define ACCLAIM_ANIMATION_H

#include <string>
#include <vector>

#include "AcclaimModelParam.h"


class AcclaimAnimation
{
    public:
        AcclaimAnimation();
        AcclaimAnimation(const std::string &filename);

        int nframes() const;
        AcclaimModelParam& frames(int i);
        const AcclaimModelParam& frames(int i) const;

        void load(const std::string &filename);

    private:
        std::vector<AcclaimModelParam> m_frames;
};

#endif
