#ifndef ADARANKER_H
#define ADARANKER_H

// An implementation of the AdaRank algorithm
//
// Shaobo Hou 
// 20.11.2011

#include "Ranker.h"

class AdaRanker : public Ranker
{
public:
    using Ranker::rank;

    // learn from a set of queries
    virtual void learn(const QueryData &data, const Metric &metric);

    // rank documents associated with a single query
    virtual RankingList rank(const QueryData::Query &data) const;

private:
    std::vector<double> weakRankerWeights;

    // use a weak ranker based on feature f to rank a single query
    RankingList weakRank(const QueryData::Query &data, const int f) const;
};

#endif
