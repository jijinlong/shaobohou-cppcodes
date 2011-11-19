#include "LinearRegressionRanker.h"

#include <fstream>

void LinearRegressionRanker::learn(const QueryData &data)
{
    TNT::Array2D<double> labels = data.label2array();
    TNT::Array2D<double>features = data.feature2array();

    learn(features, labels);

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
    TNT::Array2D<double> X = data.feature2array();

    return rank(X);
}

void LinearRegressionRanker::learn(const TNT::Array2D<double> &X, const TNT::Array2D<double> &y)
{
    TNT::Array2D<double>  Xt = transpose(X);

    params = matmult(matmult(inv(matmult(Xt, X)), Xt), y);

    std::vector<RankingPair> rankings = rank(X);
}

std::vector<RankingPair> LinearRegressionRanker::rank(const TNT::Array2D<double> &X) const
{
    TNT::Array2D<double> y = predict(X);

    std::vector<RankingPair> rankings;
    for(int i = 0; i < y.dim1(); i++)
        rankings.push_back(RankingPair(y[i][0], i));

    std::sort(rankings.rbegin(), rankings.rend());

    const int bah = 0;

    return rankings;
}

TNT::Array2D<double> LinearRegressionRanker::predict(const TNT::Array2D<double> &X) const
{
    return matmult(X, params);
}
