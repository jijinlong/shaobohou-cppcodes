#ifndef PDINV_H
#define PDINV_H

#include "jama_cholesky.h"


// B = spdinv(A)
// positive definite symmstric matrix inverse
template <typename T>
Array2D<T> spdinv(const Array2D<T> &A)
{
    assert(isSquareMatrix(A));

    JAMA::Cholesky<T> chol(A);
    Array2D<T> B(chol.solve(eye<T>(A.dim1())));
    assert(B.dim1() > 0);

    return B;
}

#endif
