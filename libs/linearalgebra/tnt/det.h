#ifndef DET_H
#define DET_H

#include "jama_lu.h"


// s = det(A)
// matrix determinant
template <typename T>
T det(const Array2D<T> &A)
{
    assert(isSquareMatrix(A));

    JAMA::LU<T> lu(A);
    return lu.det();
}

#endif
