#ifndef LINEAR_REGRESSION_RANKER
#define LIENAR_REGRESSION_RANKER

#include "Ranker.h"

#include "tnt.h"
#include "jama_lu.h"

// A = eye(dim)
template <typename T>
Array2D<T> eye(int dim)
{
    Array2D<T> A(dim, dim, T(0));
    for(int i = 0; i < dim; i++)
        A[i][i] = T(1);

    return A;
}

// B = A'
template <typename T>
Array2D<T> transpose(const Array2D<T> &A)
{
    Array2D<T> B(A.dim2(), A.dim1());
    for(int i = 0; i < A.dim1(); i++)
        for(int j = 0; j < A.dim2(); j++)
            B[j][i] = A[i][j];

    return B;
}

// B = inv(A)
template <typename T>
Array2D<T> inv(const Array2D<T> &A)
{
    assert(A.dim1()==A.dim2());

    JAMA::LU<T> lu(A);
    Array2D<T> B(lu.solve(eye<T>(A.dim1())));
    assert(B.dim1() > 0);

    return B;
}

class LinearRegressionRanker : public Ranker
{
public:
    virtual void learn(const QueryData &data);
    virtual std::vector<RankingPair> rank(const QueryData &data) const;

private:
    TNT::Array2D<double> params;

    void learn(const TNT::Array2D<double> &X, const TNT::Array2D<double> &y);
    std::vector<RankingPair> rank(const TNT::Array2D<double> &X, const std::vector<QueryVector*> &queryDocs) const;

    TNT::Array2D<double> predict(const TNT::Array2D<double> &X) const;
};

#endif
