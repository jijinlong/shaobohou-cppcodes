#include "LinearRegressionRanker.h"

#include <fstream>

void LinearRegressionRanker::learn(const QueryData &data)
{
    labels = data.label2array();
    features = data.feature2array();

    TNT::Array2D<double> &X  = features;
    TNT::Array2D<double>  Xt = transpose(X);
    TNT::Array2D<double> &y  = labels;

    TNT::Array2D<double> B = matmult(matmult(inv(matmult(Xt, X)), Xt), labels);
    params = B;

    const int nr = B.dim1();
    const int nc = B.dim2();


    /*        std::ofstream fout("X.txt");
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

void LinearRegressionRanker::rank(const QueryData &data) const
{
    TNT::Array2D<double> X = data.feature2array();
    TNT::Array2D<double> y = matmult(X, params);

    typedef std::pair<int, int> RankPair;
}
