#ifndef RANKER_H
#define RANKER_H

// Ranking algorithms
//
// Shaobo Hou 
// 20.11.2011

#include "QueryData.h"
#include "Metrics.h"

// the base ranker
class Ranker
{
public:
    Ranker() {};
    Ranker(const Metric &metric) : m_metric(&metric) {};
    virtual ~Ranker() {};

    const Metric* metric() const { return m_metric;}

    // learn from a set of queries
    virtual void learn(const QueryData &data) = 0;

    // rank documents associated with a single query
    virtual RankingList rank(const QueryData::Query &data) const = 0;

    // rank a set of queries
    std::vector<RankingList> rank(const QueryData &data) const;

protected:
    const Metric *m_metric;
};

// a ranker that does no work
class IdleRanker : public Ranker
{
public:
    using Ranker::rank;

    IdleRanker(const Metric &metric) : Ranker(metric) {};

    virtual void learn(const QueryData &data) {};
    virtual RankingList rank(const QueryData::Query &data) const;
};

#endif

