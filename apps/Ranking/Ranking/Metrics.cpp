#include "Metrics.h"

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
    double score = 0.0;
    for(unsigned int i = 0; i < rankings.size(); i++)
        score += measure(rankings[i]);

    return score/rankings.size();
}
