#include "LinearRegressionRanker.h"

#include <fstream>

TNT::Array2D<double> label2array(const std::vector<QueryVector*> &data)
{
    TNT::Array2D<double> labels(data.size(), 1);
    for(unsigned int q = 0; q < data.size(); ++q)
    {
        labels[q][0] = data[q]->relevance();
    }

    return labels;
}

TNT::Array2D<double> feature2array(const std::vector<QueryVector*> &data)
{
    int nfeature = 0;
    for(unsigned int q = 0; q < data.size(); ++q)
        nfeature = std::max(nfeature, data[q]->nfeature());

    TNT::Array2D<double> features(data.size(), nfeature+1);
    for(unsigned int q = 0; q < data.size(); ++q)
    {
        features[q][0] = 1;
        for(int j = 0; j < data[q]->nfeature(); j++)
        {
            features[q][j+1] = data[q]->features(j);
        }
    }

    return features;
}

void LinearRegressionRanker::learn(const QueryData &data)
{
    std::vector<QueryVector*> queryDocs = data.getQueryAll();
    TNT::Array2D<double> labels = label2array(queryDocs);
    TNT::Array2D<double> features = feature2array(queryDocs);

    learn(features, labels);


    std::vector<RankingPair> rankings = rank(features, queryDocs);
    double score = Metrics::MAP(rankings);

    std::vector<RankingPair> unsortedRankings;
    for(unsigned int i = 0; i < queryDocs.size(); i++)
        unsortedRankings.push_back(RankingPair(0.0, queryDocs[i]));
    double score0 = Metrics::MAP(unsortedRankings);

    /* 
    std::ofstream fout("X.txt");
    fout << X;
    fout.close();
    fout.open("y.txt");
    fout << y;
    fout.close();
    fout.open("B.txt");
    fout << B;
    fout.close();*/

    const int bah = 0;
}

std::vector<RankingPair> LinearRegressionRanker::rank(const QueryData &data) const
{
    std::vector<QueryVector*> queryDocs = data.getQueryAll();
    TNT::Array2D<double> X = feature2array(queryDocs);

    return rank(X, queryDocs);
}

void LinearRegressionRanker::learn(const TNT::Array2D<double> &X, const TNT::Array2D<double> &y)
{
    TNT::Array2D<double>  Xt = transpose(X);

    params = matmult(matmult(inv(matmult(Xt, X)), Xt), y);
}

std::vector<RankingPair> LinearRegressionRanker::rank(const TNT::Array2D<double> &X, const std::vector<QueryVector*> &queryDocs) const
{
    assert(X.dim1() == queryDocs.size());

    TNT::Array2D<double> y = predict(X);

    std::vector<RankingPair> rankings;
    for(int i = 0; i < y.dim1(); i++)
        rankings.push_back(RankingPair(y[i][0], queryDocs[i]));

    std::sort(rankings.rbegin(), rankings.rend());

    return rankings;
}

TNT::Array2D<double> LinearRegressionRanker::predict(const TNT::Array2D<double> &X) const
{
    return matmult(X, params);
}
