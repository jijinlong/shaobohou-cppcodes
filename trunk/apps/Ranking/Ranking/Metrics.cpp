#include "Metrics.h"

#include <numeric>

double AveragePrecisionMetric::measure(const RankingList &rankings) const
{
    double relSum = 0;
    double avgPNum = 0;
    double avgPDen = 0;
    for(unsigned int i = 0; i < rankings.size(); i++)
    {
        const double rel = rankings[i].second->relevance();

        relSum += rel;
        double precision = relSum/(i+1);

        avgPNum += precision*rel;
        avgPDen += rel;
    }

    return avgPNum/avgPDen;
}

double AveragePrecisionMetric::measure(const std::vector<RankingList> &rankings) const
{
    std::vector<double> scores(rankings.size(), 0);
    for(unsigned int i = 0; i < rankings.size(); i++)
        scores[i] = measure(rankings[i]);

    return std::accumulate(scores.begin(), scores.end(), 0.0)/rankings.size();
}
