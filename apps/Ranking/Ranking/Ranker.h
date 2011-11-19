#ifndef RANKER
#define RANKER

#include "QueryData.h"

 typedef std::pair<double, int> RankingPair;

class Ranker
{
public:
    Ranker() {};

    virtual ~Ranker() {};

    virtual void learn(const QueryData &data) = 0;
    virtual std::vector<RankingPair> rank(const QueryData &data) const = 0;
};

#endif

