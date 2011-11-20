#include "Metrics.h"

double Metrics::MAP(const std::vector<RankingPair> &rankings)
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
