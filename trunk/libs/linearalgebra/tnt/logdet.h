#ifndef LOGDET_H
#define LOGDET_H

#include "jama_cholesky.h"


// s = logdet(A)
// log of determinant of symmetric positive definitive matrix
template <typename T>
T logdet(const Array2D<T> &A)
{
    assert(isSquareMatrix(A));

    return 2*sum(log(diag(JAMA::Cholesky<T>(A).getL())+std::numeric_limits<T>::min()));
}

#endif
