#ifndef ADARANKER_H
#define ADARANKER_H

#include "Ranker.h"

class AdaRanker : public Ranker
{
public:
    using Ranker::rank;

    AdaRanker(const Metric &metric) : Ranker(metric) {}

    virtual void learn(const QueryData &data);
    virtual RankingList rank(const QueryData::Query &data) const;

private:
    std::vector<double> weakRankerWeights;

    RankingList weakRank(const QueryData::Query &data, const int f) const;
};

#endif
