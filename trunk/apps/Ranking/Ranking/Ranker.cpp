#include "Ranker.h"

// rank a set of queries by calling the virtual rank function on each query
std::vector<RankingList> Ranker::rank(const QueryData &data) const
{
    std::vector<RankingList> rankings(data.nquery());
    for(int q = 0; q < data.nquery(); q++)
        rankings[q] = rank(data.getQuery(q));

    return rankings;
}

// return a ranking list with changing the orders of documents
RankingList IdleRanker::rank(const QueryData::Query &data) const
{
    RankingList rankings(data.size());
    for(unsigned int i = 0; i < data.size(); i++)
        rankings[i] = RankingPair(i, data[i]);

    return rankings;
}
