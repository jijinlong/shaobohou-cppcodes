#include "Ranker.h"

std::vector<RankingList> Ranker::rank(const QueryData &data) const
{
    std::vector<RankingList> rankings(data.nquery());
    for(int q = 0; q < data.nquery(); q++)
        rankings[q] = rank(data.getQuery(q));

    return rankings;
}

RankingList IdleRanker::rank(const QueryData::Query &data) const
{
    RankingList rankings(data.size());
    for(unsigned int i = 0; i < data.size(); i++)
        rankings[i] = RankingPair(0.0, data[i]);

    return rankings;
}
