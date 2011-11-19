#include "AdaRanker.h"

#include "LinearRegressionRanker.h"

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
    LinearRegressionRanker ranker;
    /*
    std::vector<double> errors;
    for(int f = 0; f < data.nfeature(); f++)
    {
        TNT::Array2D<double> X = data.feature2array(f);
        TNT::Array2D<double> y = data.label2array();
        ranker.learn(X, y);

        TNT::Array2D<double> t = ranker.predict(X);
        TNT::Array2D<double> err2 = (t-y)*(t-y);

        errors.push_back(sum(err2));
    }
    */
    const int bah = 0;
}

std::vector<RankingPair> AdaRanker::rank(const QueryData &data) const
{
    return std::vector<RankingPair>();
}
