#ifndef METRICS_H
#define METRICS_H

#include "QueryVector.h"

typedef std::pair<double, QueryVector*> RankingPair;
typedef std::vector<RankingPair> RankingList;

class Metric
{
public:
    Metric() {};
    virtual ~Metric() {}

    virtual double measure(const RankingList &rankings) const = 0;
    virtual double measure(const std::vector<RankingList> &rankings) const = 0;
};

class AveragePrecisionMetric : public Metric
{
public:
    virtual double measure(const RankingList &rankings) const;
    virtual double measure(const std::vector<RankingList> &rankings) const;
};

#endif
