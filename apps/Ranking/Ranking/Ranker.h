#ifndef RANKER_H
#define RANKER_H

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

    virtual void learn(const QueryData &data) = 0;
    virtual RankingList rank(const QueryData::Query &data) const = 0;

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

