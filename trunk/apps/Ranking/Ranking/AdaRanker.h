#ifndef ADARANKER_H
#define ADARANKER_H

#include "Ranker.h"

class AdaRanker : public Ranker
{
public:
    virtual void learn(const QueryData &data);
    virtual std::vector<RankingPair> rank(const QueryData &data) const;

};

#endif
