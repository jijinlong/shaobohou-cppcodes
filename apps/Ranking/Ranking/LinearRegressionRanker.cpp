#include "LinearRegressionRanker.h"

#include <fstream>

#include "tnt.h"
#include "jama_lu.h"

// A = eye(dim)
TNT::Array2D<double> eye(int dim)
{
    TNT::Array2D<double> A(dim, dim, 0.0);
    for(int i = 0; i < dim; i++)
        A[i][i] = 1.0;

    return A;
}

// B = A'
TNT::Array2D<double> transpose(const TNT::Array2D<double> &A)
{
    TNT::Array2D<double> B(A.dim2(), A.dim1());
    for(int i = 0; i < A.dim1(); i++)
        for(int j = 0; j < A.dim2(); j++)
            B[j][i] = A[i][j];

    return B;
}

// B = inv(A)
TNT::Array2D<double> inv(const TNT::Array2D<double> &A)
{
    assert(A.dim1()==A.dim2());

    JAMA::LU<double> lu(A);
    TNT::Array2D<double> B(lu.solve(eye(A.dim1())));
    assert(B.dim1() > 0);

    return B;
}

// extract relevance labels into a column vector
TNT::Array2D<double> label2array(const QueryData::Query &data)
{
    TNT::Array2D<double> labels(data.size(), 1);
    for(unsigned int q = 0; q < data.size(); ++q)
    {
        labels[q][0] = data[q]->relevance();
    }

    return labels;
}

// extract features into a design matrix of form [1 F1 F2 ... Fn]
TNT::Array2D<double> feature2array(const QueryData::Query &data)
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

// extract features into a design matrix of form [1 F1 F2 ... Fn]
TNT::Array2D<double> feature2array(const QueryVector &data)
{
    TNT::Array2D<double> features(1, data.nfeature()+1);

    features[0][0] = 1;
    for(int j = 0; j < data.nfeature(); j++)
    {
        features[0][j+1] = data.features(j);
    }

    return features;
}

void LinearRegressionRanker::learn(const QueryData &data)
{
    QueryData::Query allQuerys = data.getAllQuery();

    learn(allQuerys);
}

RankingList LinearRegressionRanker::rank(const QueryData::Query &data) const
{
    TNT::Array2D<double> X = feature2array(data);

    return rank(X, data);
}

void LinearRegressionRanker::learn(const QueryData::Query &data)
{
    TNT::Array2D<double> labels = label2array(data);
    TNT::Array2D<double> features = feature2array(data);

    learn(features, labels);
}

double LinearRegressionRanker::rank(const QueryVector &data) const
{
    TNT::Array2D<double> X = feature2array(data);

    return predict(X)[0][0];
}

void LinearRegressionRanker::learn(const TNT::Array2D<double> &X, const TNT::Array2D<double> &y)
{
    TNT::Array2D<double>  Xt = transpose(X);

    // compute the parameters using the Normal equation
    params = matmult(matmult(inv(matmult(Xt, X)), Xt), y);
}

RankingList LinearRegressionRanker::rank(const TNT::Array2D<double> &X, const QueryData::Query &queryDocs) const
{
    assert(X.dim1() == queryDocs.size());

    TNT::Array2D<double> y = predict(X);

    RankingList rankings;
    for(int i = 0; i < y.dim1(); i++)
        rankings.push_back(RankingPair(y[i][0], queryDocs[i]));

    std::sort(rankings.rbegin(), rankings.rend());

    return rankings;
}

TNT::Array2D<double> LinearRegressionRanker::predict(const TNT::Array2D<double> &X) const
{
    return matmult(X, params);
}
