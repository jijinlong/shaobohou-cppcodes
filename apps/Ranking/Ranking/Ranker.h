#ifndef RANKER
#define RANKER

#include "QueryData.h"

class Ranker
{
public:
    Ranker() {};

    virtual ~Ranker() {};

    virtual void learn(const QueryData &data) = 0;
    virtual void rank(const QueryData &data) const = 0;
};

#endif

