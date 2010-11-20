#ifndef ACCLAIM_MOTION_H
#define ACCLAIM_MOTION_H

#include "AcclaimModelParam.h"

#include <vector>
#include <string>


typedef AcclaimModelParam AcclaimMove;

class AcclaimMotion
{
    public:
        AcclaimMotion();
        AcclaimMotion(const std::string &filename);

        int nmoves() const;
        AcclaimMove& moves(int i);
        const AcclaimMove& moves(int i) const;

        AcclaimMove& default_move();
        const AcclaimMove& default_move() const;

        void load(const std::string &filename);

    private:
        AcclaimMove m_default_move;
        std::vector<AcclaimMove> m_moves;
};

#endif
