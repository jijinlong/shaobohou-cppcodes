#ifndef METRICS_H
#define METRICS_H

// Various Implementations of IR Metrics
//
// Shaobo Hou
// 20.11.2011

#include "QueryVector.h"

// Ranking List typedefs
typedef std::pair<double, QueryVector*> RankingPair;
typedef std::vector<RankingPair> RankingList;

// the base metric
class Metric
{
public:
    Metric() {};
    virtual ~Metric() {}

    virtual double measure(const RankingList &rankings) const = 0;
    virtual double measure(const std::vector<RankingList> &rankings) const = 0;
};

// the mean average precision metric 
class AveragePrecisionMetric : public Metric
{
public:
    virtual double measure(const RankingList &rankings) const;
    virtual double measure(const std::vector<RankingList> &rankings) const;
};

#endif
