#ifndef ADARANKER_H
#define ADARANKER_H

#include "Ranker.h"

class AdaRanker : public Ranker
{
public:
    virtual void learn(const QueryData &data);
    virtual std::vector<RankingPair> rank(const QueryData &data) const;

private:
    std::vector<double> weakRankerWeights;

    virtual std::vector<RankingPair> rank(const std::vector<QueryVector*> &data) const;
    std::vector<RankingPair> weakRank(const std::vector<QueryVector*> &data, const int f) const;
};

#endif
