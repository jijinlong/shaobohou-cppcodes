#include "AdaRanker.h"

#include "LinearRegressionRanker.h"

#include <numeric>

// A = eye(dim)
template <typename T>
T sum(const TNT::Array2D<T> &A)
{
    T s(0);
    for(int i = 0; i < A.dim1(); i++)
        for(int j = 0; j < A.dim2(); j++)
            s += A[i][j];

    return s;
}

void AdaRanker::learn(const QueryData &data)
{
    weakRankerWeights = std::vector<double>(data.nfeature(), 0.0);

    // precompute weak rankers
    std::vector<std::vector<double> > weakRankerScores(data.nquery(), std::vector<double>(data.nfeature(), 0.0));
    for(int q = 0; q < data.nquery(); q++)
    {
        for(int f = 0; f < data.nfeature(); f++)
        {
            weakRankerScores[q][f] = Metrics::MAP(weakRank(data.getQuery(q), f));
        }
    }

    std::vector<double> scores;
    scores.push_back(Metrics::MAP(rank(data.getQueryAll())));

    // learn weak rankers
    std::vector<double> queryWeights(data.nquery(), 1.0/data.nquery());
    for(unsigned int t = 0; t < 32; t++)
    {
        std::vector<double> weightedScores(data.nfeature(), 0);
        for(int f = 0; f < data.nfeature(); f++)
        {
            if(weakRankerWeights[f] < 1e-6)
            {
                for(int q = 0; q < data.nquery(); q++)
                {
                    weightedScores[f] += weakRankerScores[q][f]*queryWeights[q];
                }
            }
        }

        // find best weak ranker
        int bestWR = 0;
        for(int f = 0; f < data.nfeature(); f++)
        {
            if(weightedScores[f] > weightedScores[bestWR])
            {
                bestWR = f;
            }
        }

        // compute alpha
        double totalWeight = std::accumulate(queryWeights.begin(), queryWeights.end(), 0.0);
        double alpha = 0.5*(log(totalWeight+weightedScores[bestWR])-log(totalWeight-weightedScores[bestWR]));
        weakRankerWeights[bestWR] = alpha;

        // update weights
        std::vector<double> queryProbs;
        for(int q = 0; q < data.nquery(); q++)
        {
            queryProbs.push_back(exp(-Metrics::MAP(rank(data.getQuery(q)))));
        }
        double totalQueryProb = std::accumulate(queryProbs.begin(), queryProbs.end(), 0.0);
        for(int q = 0; q < data.nquery(); q++)
        {
            queryWeights[q] = queryProbs[q]/totalQueryProb;
        }

        double score = Metrics::MAP(rank(data.getQueryAll()));
        scores.push_back(score);

        const int pah = 0;
    }

    const int bah = 0;
}

std::vector<RankingPair> AdaRanker::rank(const QueryData &data) const
{
    return std::vector<RankingPair>();
}

std::vector<RankingPair> AdaRanker::rank(const std::vector<QueryVector*> &data) const
{
    std::vector<RankingPair> rankings(data.size());
    for(unsigned int i = 0; i < data.size(); i++)
    {
        rankings[i] = RankingPair(0.0, data[i]);
        for(unsigned int f = 0; f < weakRankerWeights.size(); f++)
        {
            rankings[i].first += data[i]->features(f)*weakRankerWeights[f];
        }
    }

    std::sort(rankings.rbegin(), rankings.rend());

    return rankings;
}

std::vector<RankingPair> AdaRanker::weakRank(const std::vector<QueryVector*> &data, const int f) const
{
    std::vector<RankingPair> rankings(data.size());
    for(unsigned int i = 0; i < data.size(); i++)
        rankings[i] = RankingPair(data[i]->features(f), data[i]);

    std::sort(rankings.rbegin(), rankings.rend());

    return rankings;
}
