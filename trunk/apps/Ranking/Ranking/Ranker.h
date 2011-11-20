#ifndef RANKER
#define RANKER

#include "QueryData.h"
#include "Metrics.h"

// the base ranker
class Ranker
{
public:
    Ranker() {};
    Ranker(const Metric &metric) : m_metric(&metric) {};
    virtual ~Ranker() {};

    virtual void learn(const QueryData &data) = 0;
    virtual RankingList rank(const QueryData::Query &data) const = 0;

    std::vector<RankingList> rank(const QueryData &data) const
    {
        std::vector<RankingList> rankings(data.nquery());
        for(int q = 0; q < data.nquery(); q++)
            rankings[q] = rank(data.getQuery(q));

        return rankings;
    }

protected:
    const Metric *m_metric;
};

// a ranker that does no work
class IdleRanker : public Ranker
{
public:
    using Ranker::rank;

    virtual void learn(const QueryData &data) {};

    virtual RankingList rank(const QueryData::Query &data) const
    {
        RankingList rankings(data.size());
        for(unsigned int i = 0; i < data.size(); i++)
            rankings[i] = RankingPair(0.0, data[i]);

        return rankings;
    }
};

#endif

