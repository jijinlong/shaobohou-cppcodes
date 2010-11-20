#ifndef LINEAR_ALGEBRA_H
#define LINEAR_ALGEBRA_H

#include "tnt/shaobo_array1d.h"
#include "tnt/shaobo_array2d.h"
#include "tnt/shaobo_array3d.h"
#include "tnt/jama_qr.h"
#include "tnt/jama_lu.h"
#include "tnt/jama_eig.h"
#include "tnt/jama_cholesky.h"
#include "tnt/jama_svd.h"

#include "special.h"

#include <iostream>
#include <cassert>


//////////////////////////////////////////////////////////////////////////////
// logical operators
//////////////////////////////////////////////////////////////////////////////

// greaterThan(b, a, s)
template <typename T>
void greaterThan(Array1D<bool> &b, const Array1D<T> &a, const T &s)
{
    assert(a.dim() == b.dim());

    const int n = a.dim();
    for(int i = 0; i < n; i++)
        b[i] = a[i] > s;
}

// b = a > s
template <typename T>
Array1D<bool> operator>(const Array1D<T> &a, const T &s)
{
    Array1D<bool> b(a.dim());
    greaterThan(b, a, s);

    return b;
}

// B = A > s
template <typename T>
Array2D<bool> operator>(const Array2D<T> &A, const T &s)
{
    Array2D<bool> B(A.dim1(), A.dim2());
    greaterThan(B.vector(), A.vector(), s);

    return B;
}

// B = A > s
template <typename T>
Array3D<bool> operator>(const Array3D<T> &A, const T &s)
{
    Array3D<bool> B(A.dim1(), A.dim2(), A.dim3());
    greaterThan(B.vector(), A.vector(), s);

    return B;
}

// smallerThan(b, a, s)
template <typename T>
void smallerThan(Array1D<bool> &b, const Array1D<T> &a, const T &s)
{
    assert(a.dim() == b.dim());

    const int n = a.dim();
    for(int i = 0; i < n; i++)
        b[i] = a[i] < s;
}

// b = a < s
template <typename T>
Array1D<bool> operator<(const Array1D<T> &a, const T &s)
{
    Array1D<bool> b(a.dim());
    smallerThan(b, a, s);

    return b;
}


//////////////////////////////////////////////////////////////////////////////
// constructor and test functions
//////////////////////////////////////////////////////////////////////////////

template <typename T>
Array2D<T> squeeze(const Array3D<T> &A)
{
    if(A.dim1() == A.size() || A.dim2() == A.size() || A.dim3() == A.size())
        return Array2D<T>(A.size(), 1, A.vals());
    else if(A.dim1() == 1)
        return Array2D<T>(A.dim2(), A.dim3(), A.vals());
    else if(A.dim2() == 1)
        return Array2D<T>(A.dim1(), A.dim3(), A.vals());
    else if(A.dim3() == 1)
        return Array2D<T>(A.dim1(), A.dim2(), A.vals());
    else
        assert(false);
       
    return Array2D<T>();
}

template <typename T>
Array3D<T> inflate(const Array2D<T> &A, const std::string &dim)
{
    if(dim == "rows")
        return Array3D<T>(1, A.dim1(), A.dim2(), A.vals());
    else if(dim == "cols")
        return Array3D<T>(A.dim1(), 1, A.dim2(), A.vals());
    else if(dim == "mats" || dim == "") 
        return Array3D<T>(A.dim1(), A.dim2(), 1, A.vals());
    else
        assert(false);

    return Array3D<T>();
}

// A = eye(dim)
template <typename T>
Array2D<T> eye(int dim)
{
    Array2D<T> A(dim, dim, T(0));
    for(int i = 0; i < dim; i++)
        A(i, i) = T(1);

    return A;
}

// B = zeros(dim1, dim2)
template <typename T>
Array2D<T> zeros(int dim1, int dim2)
{
    return Array2D<T>(dim1, dim2, T(0));
}

// B = zeros(dim)
template <typename T>
Array2D<T> zeros(int dim)
{
    return zeros<T>(dim, dim);
}

// B = ones(dim1, dim2)
template <typename T>
Array2D<T> ones(int dim1, int dim2)
{
    return Array2D<T>(dim1, dim2, T(1));
}

// B = zeros(dim)
template <typename T>
Array2D<T> ones(int dim)
{
    return ones<T>(dim, dim);
}

// B = rand(dim1, dim2)
template <typename T>
Array2D<T> rand(int dim1, int dim2)
{
    Array2D<T> B(dim1, dim2);

    int n = B.size();
    for(int i = 0; i < n; i++)
        B(i) = uniform_rand();

    return B;
}

// B = rand(dim)
template <typename T>
Array2D<T> rand(int dim)
{
    return rand<T>(dim, dim);
}

// B = randn(dim1, dim2)
template <typename T>
Array2D<T> randn(int dim1, int dim2)
{
    Array2D<T> B(dim1, dim2);

    int n = B.size();
    for(int i = 0; i < n; i++)
        B(i) = normal_rand();

    return B;
}

// B = randn(dim)
template <typename T>
Array2D<T> randn(int dim)
{
    return randn<T>(dim, dim);
}

// test square matrix
template <typename T>
bool isSquareMatrix(const Array2D<T> &A)
{
    return A.dim1() == A.dim2();
}


//////////////////////////////////////////////////////////////////////////////
// template vector elementwise operators
//////////////////////////////////////////////////////////////////////////////

// add(c, b, a)
template <typename T>
void add(Array1D<T> &c, const Array1D<T> &a, const Array1D<T> &b)
{
    assert(c.dim() == a.dim());
    assert(c.dim() == b.dim());

    int n = a.dim();
    for (int i=0; i<n; i++)
        c[i] = a[i]+b[i];
}

// c = a + b
template <typename T>
Array1D<T> operator+(const Array1D<T> &a, const Array1D<T> &b)
{
    assert(a.dim() == b.dim());

    Array1D<T> c(a.dim());
    add(c, a, b);

    return c;
}

// sub(c, b, a)
template <typename T>
void sub(Array1D<T> &c, const Array1D<T> &a, const Array1D<T> &b)
{
    assert(c.dim() == a.dim());
    assert(c.dim() == b.dim());

    int n = a.dim();
    for (int i=0; i<n; i++)
        c[i] = a[i]-b[i];
}

// c = a - b
template <typename T>
Array1D<T> operator-(const Array1D<T> &a, const Array1D<T> &b)
{
    assert(a.dim() == b.dim());

    Array1D<T> c(a.dim());
    sub(c, a, b);

    return c;
}

// dotmult(c, b, a)
template <typename T>
void dotmult(Array1D<T> &c, const Array1D<T> &a, const Array1D<T> &b)
{
    assert(c.dim() == a.dim());
    assert(c.dim() == b.dim());

    int n = a.dim();
    for (int i=0; i<n; i++)
        c[i] = a[i]*b[i];
}

// c = a .* b
template <typename T>
Array1D<T> dotmult(const Array1D<T> &a, const Array1D<T> &b)
{
    assert(a.dim() == b.dim());

    Array1D<T> c(a.dim());
    dotmult(c, a, b);

    return c;
}

// dotdiv(c, b, a)
template <typename T>
void dotdiv(Array1D<T> &c, const Array1D<T> &a, const Array1D<T> &b)
{
    assert(c.dim() == a.dim());
    assert(c.dim() == b.dim());

    int n = a.dim();
    for (int i=0; i<n; i++)
        c[i] = a[i]/b[i];
}

// c = a ./ b
template <typename T>
Array1D<T> dotdiv(const Array1D<T> &a, const Array1D<T> &b)
{
    assert(a.dim() == b.dim());

    Array1D<T> c(a.dim());
    dotdiv(c, a, b);

    return c;
}

// a = a + b
template <typename T>
Array1D<T>&  operator+=(Array1D<T> &a, const Array1D<T> &b)
{
    assert(a.dim() == b.dim());

    add(a, a, b);

    return a;
}


//////////////////////////////////////////////////////////////////////////////
// matrix elementwise operators
//////////////////////////////////////////////////////////////////////////////

// C = A + B
template <typename T>
Array2D<T> operator+(const Array2D<T> &A, const Array2D<T> &B)
{
    assert(A.dim1() == B.dim1());
    assert(A.dim2() == B.dim2());

    Array2D<T> C(A.dim1(), A.dim2());
    add(C.vector(), A.vector(), B.vector());

    return C;
}

// C = A - B
template <typename T>
Array2D<T> operator-(const Array2D<T> &A, const Array2D<T> &B)
{
    assert(A.dim1() == B.dim1());
    assert(A.dim2() == B.dim2());

    Array2D<T> C(A.dim1(), A.dim2());
    sub(C.vector(), A.vector(), B.vector());

	return C;
}

// C = A .* B
template <typename T>
Array2D<T> dotmult(const Array2D<T> &A, const Array2D<T> &B)
{
	assert(A.dim1() == B.dim1());
    assert(A.dim2() == B.dim2());

	Array2D<T> C(A.dim1(), A.dim2());
	dotmult(C.vector(), A.vector(), B.vector());

	return C;
}

// C = A ./ B
template <typename T>
Array2D<T> dotdiv(const Array2D<T> &A, const Array2D<T> &B)
{
	assert(A.dim1() == B.dim1());
    assert(A.dim2() == B.dim2());

	Array2D<T> C(A.dim1(), A.dim2());
	dotdiv(C.vector(), A.vector(), B.vector());

	return C;
}

// A = A + B
template <typename T>
Array2D<T>& operator+=(Array2D<T> &A, const Array2D<T> &B)
{
	assert(A.dim1() == B.dim1());
    assert(A.dim2() == B.dim2());

    add(A.vector(), A.vector(), B.vector());

    return A;
}


//////////////////////////////////////////////////////////////////////////////
// matrix matrix operators
//////////////////////////////////////////////////////////////////////////////

// C = A * B
template <typename T>
Array2D<T> operator*(const Array2D<T> &A, const Array2D<T> &B)
// Array2D<T> matmult(const Array2D<T> &A, const Array2D<T> &B)
{
    assert(A.dim2() == B.dim1());

    int M = A.dim1();
    int N = A.dim2();
    int K = B.dim2();

    Array2D<T> C(M, K);
    for(int i = 0; i < M; i++)
        for(int j = 0; j < K; j++)
        {
            T sum = 0;

            for(int k = 0; k < N; k++)
                sum += A(i, k)*B(k, j);

            C(i, j) = sum;
        }

    return C;
}

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


//////////////////////////////////////////////////////////////////////////////
// matrix vector operators
//////////////////////////////////////////////////////////////////////////////

// c = A * b
template <typename T>
Array1D<T> operator*(const Array2D<T> &A, const Array1D<T> &b)
{
    assert(A.dim2() == b.dim());

    int n1 = A.dim1(), n2 = A.dim2();

    Array1D<T> c(n1, 0.0);
    for(int i = 0; i < n1; i++)
        for(int j = 0; j < n2; j++)
            c[i] += A(i, j) * b[j];

    return c;
}

// c = a * B
template <typename T>
Array1D<T> operator*(const Array1D<T> &a, const Array2D<T> &B)
{
    assert(a.dim() == B.dim1());

    int n1 = B.dim1(), n2 = B.dim2();

    Array1D<T> c(n2, 0.0);
    for(int i = 0; i < n2; i++)
        for(int j = 0; j < n1; j++)
            c[i] += a[j] * B(j, i);

    return c;
}


//////////////////////////////////////////////////////////////////////////////
// vector scalar operators
//////////////////////////////////////////////////////////////////////////////

// negate(b, a)
template <typename T>
void negate(Array1D<T> &b, const Array1D<T> &a)
{
    assert(a.dim() == b.dim());

    int n = a.dim();
    for (int i=0; i<n; i++)
        b[i] = -a[i];
}

// b = -a
template <typename T>
Array1D<T> operator-(const Array1D<T> &a)
{
    Array1D<T> b(a.dim());
    negate(b, a);

    return b;
}

// add(b, a, s)
template <typename T>
void add(Array1D<T> &b, const Array1D<T> &a, const T &s)
{
    assert(a.dim() == b.dim());

	int n = a.dim();
	for (int i=0; i<n; i++)
		b[i] = a[i]+s;
}

// b = a + s
template <typename T>
Array1D<T> operator+(const Array1D<T> &a, const T &s)
{
    Array1D<T> b(a.dim());
    add(b, a, s);

	return b;
}

// sub(b, s, a)
template <typename T>
void sub(Array1D<T> &b, const T &s, const Array1D<T> &a)
{
    assert(a.dim() == b.dim());

	int n = a.dim();
	for (int i=0; i<n; i++)
		b[i] = s-a[i];
}

// b = s - a
template <typename T>
Array1D<T> operator-(const T &s, const Array1D<T> &a)
{
    Array1D<T> b(a.dim());
    sub(b, s, a);

	return b;
}

// sub(b, a, s)
template <typename T>
void sub(Array1D<T> &b, const Array1D<T> &a, const T &s)
{
    assert(a.dim() == b.dim());

	int n = a.dim();
	for (int i=0; i<n; i++)
		b[i] = a[i]-s;
}

// b = s - a
template <typename T>
Array1D<T> operator-(const Array1D<T> &a, const T &s)
{
    Array1D<T> b(a.dim());
    sub(b, a, s);

	return b;
}

// dotmult(b, a, s)
template <typename T>
void dotmult(Array1D<T> &b, const Array1D<T> &a, const T &s)
{
    assert(a.dim() == b.dim());

    int n = a.dim();
    for(int i = 0; i < n; i++)
        b[i] = a[i]*s;
}

// b = a * s
template <typename T>
Array1D<T> operator*(const Array1D<T> &a, const T &s)
{
    Array1D<T> b(a.dim());
    dotmult(b, a, s);

    return b;
}

// b = s * a
template <typename T>
Array1D<T> operator*(const T &s, const Array1D<T> &a)
{
    return a*s;
}

// b = a ./ s
template <typename T>
Array1D<T> operator/(const Array1D<T> &a, const T &s)
{
	return a*(1.0/s);
}

// dotdiv(b, a, s)
template <typename T>
void dotdiv(Array1D<T> &b, const T &s, const Array1D<T> &a)
{
    assert(a.dim() == b.dim());

    int n = a.dim();
    for(int i = 0; i < n; i++)
        b[i] = s/a[i];
}

// b = s ./ a
template <typename T>
Array1D<T> operator/(const T &s, const Array1D<T> &a)
{
    Array1D<T> b(a.dim());
    dotdiv(b, s, a);

    return b;
}


//////////////////////////////////////////////////////////////////////////////
// matrix scalar operators
//////////////////////////////////////////////////////////////////////////////

// B = -A
template <typename T>
Array2D<T> operator-(const Array2D<T> &A)
{
    Array2D<T> B(A.dim1(), A.dim2());
    negate(B.vector(), A.vector());

    return B;
}

// B = A + s
template <typename T>
Array2D<T> operator+(const Array2D<T> &A, const T &s)
{
    Array2D<T> B(A.dim1(), A.dim2());
    add(B.vector(), A.vector(), s);

    return B;
}

// B = A - s
template <typename T>
Array2D<T> operator-(const Array2D<T> &A, const T &s)
{
    Array2D<T> B(A.dim1(), A.dim2());
    sub(B.vector(), A.vector(), s);

    return B;
}

// B = s - A
template <typename T>
Array2D<T> operator-(const T &s, const Array2D<T> &A)
{
    Array2D<T> B(A.dim1(), A.dim2());
    sub(B.vector(), s, A.vector());

    return B;
}

// B = A * s
template <typename T>
Array2D<T> operator*(const Array2D<T> &A, const T &s)
{
    Array2D<T> B(A.dim1(), A.dim2());
    dotmult(B.vector(), A.vector(), s);

    return B;
}

// B = s * A
template <typename T>
Array2D<T> operator*(const T &s, const Array2D<T> &A)
{
    return A*s;
}

// B = A ./ s
template <typename T>
Array2D<T> operator/(const Array2D<T> &A, const T &s)
{
	return A*(1.0/s);
}

// B = s ./ A
template <typename T>
Array2D<T> operator/(const T &s, const Array2D<T> &A)
{
    Array2D<T> B(A.dim1(), A.dim2());
    dotdiv(B.vector(), s, A.vector());

    return B;
}


//////////////////////////////////////////////////////////////////////////////
// trace and more utility functions
//////////////////////////////////////////////////////////////////////////////

// s = trace(A)
template <typename T>
T trace(const Array2D<T> &A)
{
    assert(isSquareMatrix(A));

    int n = A.dim1();

    T res(0);
    for(int i = 0; i < n; i++)
        res += A(i, i);

    return res;
}

// s = trace(A * B)
template <typename T>
T trace_matmult(const Array2D<T> &A, const Array2D<T> &B)
{
    assert(A.dim1() == B.dim2());
    assert(A.dim2() == B.dim1());

    return sum(sum(dotmult(A, transpose(B))))(0, 0);
}

// C = dist2(A, B);
template <typename T>
Array2D<T> dist2(const Array2D<T> &A, const Array2D<T> &B)
{
    assert(A.dim2() == B.dim2());

    return repmat(sum(dotmult(A, A), "cols"), 1, B.dim1()) +
           repmat(transpose(sum(dotmult(B, B), "cols")), A.dim1(), 1) - 2.0*(A*transpose(B));
}


//////////////////////////////////////////////////////////////////////////////
// sum, prod, cumsum, mean and max operators
//////////////////////////////////////////////////////////////////////////////

// B = sum(A, dim)
template <typename T>
Array2D<T> sum(const Array2D<T> &A, const std::string &dim="")
{
    if(A.dim1() == 0 && A.dim2() == 0)
        return Array2D<T>();

    const int nm = A.dim1()*A.dim2();

    Array2D<T> B;
    if(((A.dim1() == 1) || (A.dim2() == 1)) && (dim == ""))
    {
        B = Array2D<T>(1, 1, T(0));
        for(int i = 0; i < nm; i++)
            B(0) += A(i);
    }
    else if((dim == "rows") || (dim == ""))
    {
        B = Array2D<T>(1, A.dim2(), T(0));
        for(int i = 0; i < A.dim1(); i++)
            for(int j = 0; j < A.dim2(); j++)
                B(j) += A(i, j);
    }
    else if(dim == "cols")
    {
        B = Array2D<T>(A.dim1(), 1, T(0));
        for(int i = 0; i < A.dim1(); i++)
            for(int j = 0; j < A.dim2(); j++)
                B(i) += A(i, j);
    }
    else
        assert(false);

    return B;
}

// B = cumsum(A, dim)
template <typename T>
Array2D<T> cumsum(const Array2D<T> &A, const std::string &dim="rows")
{
    if(A.dim1() == 0 && A.dim2() == 0)
        return Array2D<T>();

    Array2D<T> B(A.dim1(), A.dim2());
    if(dim == "rows")
    {
        for(int j = 0; j < A.dim2(); j++)
            B(0, j) = A(0, j);
        for(int i = 1; i < A.dim1(); i++)
            for(int j = 0; j < A.dim2(); j++)
                B(i, j) = B(i-1, j) + A(i, j);
    }
    else if(dim == "cols")
    {
        for(int i = 0; i < A.dim1(); i++)
        {
            B(i, 0) = A(i, 0);
            for(int j = 1; j < A.dim2(); j++)
                B(i, j) = B(i, j-1) + A(i, j);
        }
    }
    else
        assert(false);

    return B;
}

// B = mean(A, dim)
template <typename T>
Array2D<T> mean(const Array2D<T> &A, const std::string &dim="")
{
    if(A.dim1() == 0 && A.dim2() == 0)
        return Array2D<T>();

    Array2D<T> B;
    if(((A.dim1() == 1) || (A.dim2() == 1)) && (dim == ""))
    {
        B = Array2D<T>(1, 1, T(0));
        for(int i = 0; i < A.dim1(); i++)
            for(int j = 0; j < A.dim2(); j++)
                B(0, 0) += A(i, j);
        B = B/T(std::max(A.dim1(), A.dim2()));
    }
    else if((dim == "rows") || (dim == ""))
    {
        B = Array2D<T>(1, A.dim2(), T(0));
        for(int i = 0; i < A.dim1(); i++)
            for(int j = 0; j < A.dim2(); j++)
                B(0, j) += A(i, j);

        B = B/T(A.dim1());
    }
    else if(dim == "cols")
    {
        B = Array2D<T>(A.dim1(), 1, T(0));
        for(int i = 0; i < A.dim1(); i++)
            for(int j = 0; j < A.dim2(); j++)
                B(i, 0) += A(i, j);

        B = B/T(A.dim2());
    }
    else
        assert(false);

    return B;
}

// s = mean(a)
template <typename T>
T mean(const Array1D<T> &a)
{
    assert(a.dim1() > 0);

    T m(0);
    for(int i = 0; i < a.dim(); i++)
        m += a[i];
    m /= static_cast<T>(a.dim());

    return m;
}

// B = max(A, dim)
template <typename T>
Array1D<T> max(const Array2D<T> &A, const std::string &dim="")
{
    assert(A.dim1() > 0);
    assert(A.dim2() > 0);

    Array1D<T> B;
    if(((A.dim1() == 1) || (A.dim2() == 1)) && (dim == ""))
    {
        B = Array1D<T>(1, T(0));
        B[0] = A(0, 0);
        for(int i = 0; i < A.dim1(); i++)
            for(int j = 0; j < A.dim2(); j++)
                if(A(i, j) > B[0])
                    B[0] = A(i, j);
    }
    else if((dim == "rows") || (dim == ""))
    {
        B = Array1D<T>(A.dim2(), T(0));
        for(int j = 0; j < A.dim2(); j++)
            B[j] = A(0, j);
        for(int i = 1; i < A.dim1(); i++)
            for(int j = 0; j < A.dim2(); j++)
                if(A(i, j) > B[j])
                    B[j] = A(i, j);
    }
    else if(dim == "cols")
    {
        B = Array1D<T>(A.dim1(), T(0));
        for(int i = 0; i < A.dim1(); i++)
            B[i] = A(i, 0);
        for(int i = 0; i < A.dim1(); i++)
            for(int j = 0; j < A.dim2(); j++)
                if(A(i, j) > B[i])
                    B[i] = A(i, j);
    }
    else
        assert(false);

    return B;
}

// B = min(A, dim)
template <typename T>
Array1D<T> min(const Array2D<T> &A, const std::string &dim="")
{
    if(A.dim1() == 0 && A.dim2() == 0)
        return Array1D<T>();

    Array1D<T> B;
    if(((A.dim1() == 1) || (A.dim2() == 1)) && (dim == ""))
    {
        B = Array1D<T>(1, T(0));
        B[0] = A(0, 0);
        for(int i = 0; i < A.dim1(); i++)
            for(int j = 0; j < A.dim2(); j++)
                if(A(i, j) < B[0])
                    B[0] = A(i, j);
    }
    else if((dim == "rows") || (dim == ""))
    {
        B = Array1D<T>(A.dim2(), T(0));
        for(int j = 0; j < A.dim2(); j++)
            B[j] = A(0, j);
        for(int i = 1; i < A.dim1(); i++)
            for(int j = 0; j < A.dim2(); j++)
                if(A(i, j) < B[j])
                    B[j] = A(i, j);
    }
    else if(dim == "cols")
    {
        B = Array1D<T>(A.dim1(), T(0));
        for(int i = 0; i < A.dim1(); i++)
            B[i] = A(i, 0);
        for(int i = 0; i < A.dim1(); i++)
            for(int j = 0; j < A.dim2(); j++)
                if(A(i, j) < B[i])
                    B[i] = A(i, j);
    }
    else
        assert(false);

    return B;
}

// s = sum(a)
template <typename T>
T sum(const Array1D<T> &a)
{
    int n = a.dim();

    T s(0);
    for(int i = 0; i < n; i++)
        s += a[i];
    return s;
}

// s = prod(a)
template <typename T>
T prod(const Array1D<T> &a)
{
    int n = a.dim();

    assert(n > 0);

    T prods(1);
    for(int i = 0; i < n; i++)
        prods *= a[i];
    return prods;
}

// s = max(a)
template <typename T>
T max(const Array1D<T> &a)
{
    assert(a.dim() > 0);

    int n = a.dim();

    T s(a[0]);
    for(int i = 1; i < n; i++)
        if(a[i] > s)
            s = a[i];
    return s;
}

// s = min(a)
template <typename T>
T min(const Array1D<T> &a)
{
    assert(a.dim() > 0);

    int n = a.dim();

    T s(a[0]);
    for(int i = 1; i < n; i++)
        if(a[i] < s)
            s = a[i];
    return s;
}


//////////////////////////////////////////////////////////////////////////////
// cov, dot and distance operators
//////////////////////////////////////////////////////////////////////////////

// B = cov(A)
template <typename T>
Array2D<T> cov(const Array2D<T> &A)
{
    if(A.dim1() == 0 && A.dim2() == 0)
        return Array2D<T>();

    Array2D<T> mu = mean(A);
    return (transpose(A)*A)/T(A.dim1()) - transpose(mu)*mu;
}

// B = var(A)
template <typename T>
Array2D<T> var(const Array2D<T> &A)
{
    Array2D<T> mu = mean(A);
    return sum(dotmult(A, A))/T(A.dim1()) - dotmult(mu, mu);
}

// s = dot(a, b)
template <typename T>
T dot(const Array1D<T> &a, const Array1D<T> &b)
{
    assert(a.dim() == b.dim());

    int n = a.dim();

    T s(0.0);
    for(int i = 0; i < n; i++)
        s += a[i]*b[i];
    return s;
}

// s = b'*A*b
template <typename T>
T mahalanobisDistance(const Array2D<T> &A, const Array1D<T> &b)
{
    return dot(b*A, b);
}

// s = diag(B'*A*B)
template <typename T>
Array2D<T> mahalanobisDistance(const Array2D<T> &A, const Array2D<T> &B)
{
    return sum(dotmult(transpose(B)*A, B), "cols");
}


//////////////////////////////////////////////////////////////////////////////
// inverse and determinant operators
//////////////////////////////////////////////////////////////////////////////

// B = inv(A)
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
    assert(isSquareMatrix(A));

    JAMA::LU<T> lu(A);
    Array2D<T> B(lu.solve(eye<T>(A.dim1())));
    assert(B.dim1() > 0);

    return B;
}

// s = det(A)
template <typename T>
T det(const Array2D<T> &A)
{
    assert(isSquareMatrix(A));

    JAMA::LU<T> lu(A);
    return lu.det();
}

// B = pdinv(A)
template <typename T>
Array2D<T> pdinv(const Array2D<T> &A)
{
    assert(isSquareMatrix(A));

    JAMA::Cholesky<T> chol(A);
    Array2D<T> B(chol.solve(eye<T>(A.dim1())));
    assert(B.dim1() > 0);

    return B;
}

// B = pinv(A)
// B is the premultiply inverse, i.e. B*A == I
template <typename T>
Array2D<T> pinv(const Array2D<T> &A, bool include_nullspace=false)
{
    JAMA::SVD<T> svd(A);
    Array2D<T> U = svd.getU();
    Array2D<T> S = svd.getS();
    Array2D<T> V = svd.getV();
    Array1D<T> s = diag(S);

    int count = 0;
    T tol(std::min(S.dim1(), S.dim2())*2.2204e-16*max(diag(S)));
    for(int i = 0; i < s.dim(); i++)
        if(s[i] > tol)
        {
            s[i] = T(1.0)/s[i];
            count++;
        }
        else
            s[i] = 0.0;

    assert(count > 0);
    Array2D<double> inv1 = getcols(V, 0, count-1) * diag(s) * transpose(getcols(U, 0, count-1));
    if(include_nullspace)
    {
        Array2D<double> inv2(inv1.dim1(), S.dim2());
        for(int i = 0; i < inv1.dim1(); i++)
        {
            for(int j = 0; j < inv1.dim2(); j++)
                inv2(i, j) = inv1(i, j);
            for(int j = inv1.dim2(); j < inv2.dim2(); j++)
                inv2(i, j) = V(i, j);
        }

        return inv2;
    }
    else
        return inv1;
}


//////////////////////////////////////////////////////////////////////////////
// vector covariance operators
//////////////////////////////////////////////////////////////////////////////

// B = cov(a)
template <typename T>
Array2D<T> covariance(const Array1D<T> &a)
{
    int n = a.dim();

    Array2D<T> B(n, n);
    for(int i = 0; i < n; i++)
        for(int j = 0; j < n; j++)
            B(i, j) = a[i]*a[j];

    return B;
}


//////////////////////////////////////////////////////////////////////////////
// overloaded maths operators
//////////////////////////////////////////////////////////////////////////////

// b = log(a)
template <typename T>
Array1D<T> log(const Array1D<T> &a)
{
    Array1D<T> b(a.dim());
    for(int i = 0; i < a.dim(); i++)
        b[i] = log(a[i]);

    return b;
}

// B = log(a)
template <typename T>
Array2D<T> log(const Array2D<T> &A)
{
    int n = A.dim1()*A.dim2();

    Array2D<T> B(A.dim1(), A.dim2());
    for(int i = 0; i < n; i++)
        B(i) = log(A(i));

    return B;
}

// B = exp(a)
template <typename T>
Array2D<T> exp(const Array2D<T> &A)
{
    int n = A.dim1()*A.dim2();

    Array2D<T> B(A.dim1(), A.dim2());
    for(int i = 0; i < n; i++)
        B(i) = exp(A(i));

    return B;
}

// sqrt(b, a)
template <typename T>
void sqrt(Array1D<T> &b, const Array1D<T> &a)
{
    assert(b.dim() == a.dim());

    const int n = a.dim();
    for(int i = 0; i < n; i++)
        b[i] = sqrt(a[i]);
}

// b = sqrt(a)
template <typename T>
Array1D<T> sqrt(const Array1D<T> &a)
{
    Array1D<T> b(a.dim());
    sqrt(b, a);

    return b;
}

// B = sqrt(A)
template <typename T>
Array2D<T> sqrt(const Array2D<T> &A)
{
    Array2D<T> B(A.dim1(), A.dim2());
    sqrt(B.vector(), A.vector());

    return B;
}

// fabs(b, a)
template <typename T>
void fabs(Array1D<T> &b, const Array1D<T> &a)
{
    assert(b.dim() == a.dim());

    int n = a.dim();
    for (int i = 0; i < n; i++)
        b[i] = fabs(a[i]);
}

// b = fabs(a)
template <typename T>
Array1D<T> fabs(const Array1D<T> &a)
{
    Array1D<T> b(a.dim());
    fabs(b, a);

    return b;
}

// B = fabs(A)
template <typename T>
Array2D<T> fabs(const Array2D<T> &A)
{
    Array2D<T> B(A.dim1(), A.dim2());
    fabs(B.vector(), A.vector());

    return B;
}

//////////////////////////////////////////////////////////////////////////////
// logdet operator
//////////////////////////////////////////////////////////////////////////////

// s = logdet(A)
template <typename T>
T logdet(const Array2D<T> &A)
{
    assert(isSquareMatrix(A));

    return 2*sum(log(diag(JAMA::Cholesky<T>(A).getL())+std::numeric_limits<T>::min()));
}



//////////////////////////////////////////////////////////////////////////////
// tensor operators
//////////////////////////////////////////////////////////////////////////////

// C = A - B
template <typename T>
Array3D<T> operator-(const Array3D<T> &A, const Array3D<T> &B)
{
    assert(A.dim1() == B.dim1());
    assert(A.dim2() == B.dim2());
    assert(A.dim3() == B.dim3());

    Array3D<T> C(A.dim1(), A.dim2(), A.dim3());
    sub(C.vector(), A.vector(), B.vector());

    return C;
}

// A = A + B
template <typename T>
Array3D<T>&  operator+=(Array3D<T> &A, const Array3D<T> &B)
{
    assert(A.dim1() == B.dim1());
    assert(A.dim2() == B.dim2());
    assert(A.dim3() == B.dim3());	

    add(A.vector(), A.vector(), B.vector());

    return A;
}

// B = s - A
template <typename T>
Array3D<T> operator-(const T &s, const Array3D<T> &A)
{
    Array3D<T> B(A.dim1(), A.dim2(), A.dim3());
    sub(B.vector(), s, A.vector());

    return B;
}

// B = A * s
template <typename T>
Array3D<T> operator*(const Array3D<T> &A, const T &s)
{
    Array3D<T> B(A.dim1(), A.dim2(), A.dim3());
    dotmult(B.vector(), A.vector(), s);

    return B;
}

// B = A ./ s
template <typename T>
Array3D<T> operator/(const Array3D<T> &A, const T &s)
{
	return A*(1.0/s);
}

// B = sqrt(A)
template <typename T>
Array3D<T> sqrt(const Array3D<T> &A)
{
    Array3D<T> B(A.dim1(), A.dim2(), A.dim3());
    sqrt(B.vector(), A.vector());

    return B;
}

// B = fabs(A)
template <typename T>
Array3D<T> fabs(const Array3D<T> &A)
{
    Array3D<T> B(A.dim1(), A.dim2(), A.dim3());
    fabs(B.vector(), A.vector());

    return B;
}

// B = sum3(A)
template <typename T>
Array3D<T> sum3(const Array3D<T> &A)
{
    assert(A.dim1() > 0);
    assert(A.dim2() > 0);
    assert(A.dim3() > 0);

    const int nm = A.dim1()*A.dim2();

    Array3D<T> B(A.dim1(), A.dim2(), 1);

    T *psource = A.vals();
    T *ptarget = B.vals();
    for(int k = 0; k < A.dim3(); k++)
    {
        ptarget = B.vals();
        for(int i = 0; i < nm; i++)
        {
            *ptarget += *psource;
            psource++;
            ptarget++;
        }
    }

    return B;
}



//////////////////////////////////////////////////////////////////////////////
// some special functions used by clustering with errors code
//////////////////////////////////////////////////////////////////////////////

// increment and decrement diagonal
template <typename T>
Array2D<T> increment_diagonal(const Array2D<T> &mat, const Array1D<T> &vec)
{
    assert(isSquareMatrix(mat));
    assert(mat.dim1() == vec.dim());

    Array2D<T> res(mat.dim1(), mat.dim2());
    for(int i = 0; i < mat.dim1(); i++)
        for(int j = 0; j < mat.dim2(); j++)
            if(i == j)
                res(i, j) = mat(i, j) + vec[i];
            else
                res(i, j) = mat(i, j);

    return res;
}

// clamp eigenvalues
template <typename T>
int isSPDVector(const Array1D<T> &vec, const T &reg)
{
    int neg_count = 0;
    for(int j = 0; j < vec.dim(); j++)
        if(vec[j] < reg)
            neg_count--;

    return neg_count;
}

// clamp eigenvalues
template <typename T>
int isSPDMatrix(const Array2D<T> &mat, const T &reg)
{
    assert(mat.dim1() == mat.dim2());

    JAMA::Eigenvalue<T> eig(mat);
    Array2D<T> eval = eig.getD();

    int neg_count = 0;
    for(int j = 0; j < eval.dim1(); j++)
        if(eval(j, j) < reg)
            neg_count--;

    return neg_count;
}

template <typename T>
Array2D<T> clampEigenValues(const Array2D<T> &mat, const T &reg)
{
    JAMA::Eigenvalue<T> eig(mat);
    Array2D<T> eval = eig.getD();
    Array2D<T> evec = eig.getV();

    unsigned int count = 0;
    for(int i = 0; i < eval.dim1(); i++)
        if(eval(i, i) < reg)
        {
            eval(i, i) = reg;
            count++;
        }

    if(count > 0)
        return evec * eval * transpose(evec);
    else
        return mat;
}

template <typename T>
Array1D<T> clampElementValues(const Array1D<T> &A, const T &reg)
{
    Array1D<T> B(A);
    for(int i = 0; i < A.dim(); i++)
        if(A[i] < reg)
                B[i] = reg;

    return B;
}

#endif
