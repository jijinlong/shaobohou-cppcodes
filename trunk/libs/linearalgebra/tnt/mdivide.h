#ifndef MDIVIDE_H
#define MDIVIDE_H

#include "jama_lu.h"


// C = mldivide(A, B)
// equivalent to C = A \ B
template <typename T>
Array2D<T> mldivide(const Array2D<T> &A, const Array2D<T> &B)
{
    assert(A.dim1() == B.dim1());

    JAMA::LU<T> lu(A);

    return lu.solve(B);
}

// C = mrdivide(A, B) = mldivide(B', A')
// equivalent to C = A / B
template <typename T>
Array2D<T> mrdivide(const Array2D<T> &A, const Array2D<T> &B)
{
    return mldivide(transpose(B), transpose(A));
}

#endif
