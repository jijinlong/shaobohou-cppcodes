#ifndef INV_H
#define INV_H

#include "jama_lu.h"


// B = inv(A)
// matrix inverse
template <typename T>
Array2D<T> inv(const Array2D<T> &A, T &det)
{
    assert(isSquareMatrix(A));

	JAMA::LU<T> lu(A);
    det = lu.det();
    Array2D<T> B(lu.solve(eye<T>(A.dim1())));
    assert(B.dim1() > 0);

	return B;
}

// B = inv(A)
template <typename T>
Array2D<T> inv(const Array2D<T> &A)
{
    assert(square(A));

    JAMA::LU<T> lu(A);
    Array2D<T> B(lu.solve(eye<T>(A.dim1())));
    assert(B.dim1() > 0);

    return B;
}

#endif
