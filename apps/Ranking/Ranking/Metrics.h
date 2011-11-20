#ifndef METRICS_H
#define METRICS_H

#include "QueryVector.h"

typedef std::pair<double, QueryVector*> RankingPair;

namespace Metrics
{
    double MAP(const std::vector<RankingPair> &rankings);
}

#endif
