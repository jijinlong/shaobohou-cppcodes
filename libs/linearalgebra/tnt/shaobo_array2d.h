#ifndef ARRAY2D_H
#define ARRAY2D_H

#include "shaobo_array1d.h"


namespace TNT
{

template <typename T>
class Array2D
{
    public:
        Array2D();
        Array2D(int nr, int nc);
        Array2D(int nr, int nc, const T &a);
        Array2D(int nr, int nc, const T *a);
        Array2D(const Array2D &other);
        ~Array2D();

        inline Array2D& operator=(const Array2D &rhs);
        inline Array2D& operator=(const T &a);

        inline Array2D& set(const Array2D<bool> &flags, const T &a);

        inline T& operator()(int i);
        inline const T& operator()(int i) const;
        inline T& operator()(int i, int j);
        inline const T& operator()(int i, int j) const;

        inline int dim1() const;
        inline int dim2() const;
        inline int size() const;

        inline void reshape(int nr, int nc);
        inline const Array2D<T>& copy() const; // for compatibility with original TNT

        inline Array1D<T>& vector();
        inline const Array1D<T>& vector() const;

        inline T* vals();
        inline T* vals() const;

    private:
        int nrows;
        int ncols;

        Array1D<T> data;
};


// IO functions
template <typename T> std::ostream& operator<<(std::ostream &s, const Array2D<T> &A);
template <typename T> std::istream& operator>>(std::istream &s, Array2D<T> &A);
template <typename T> void saveArray2D(const Array2D<T> &A, const std::string &filename);
template <typename T> Array2D<T> loadArray2D(const std::string &filename);

// subarray functions
template <typename T> Array2D<T> getsub(const Array2D<T> &A, int r0, int r1, int c0, int c1);
template <typename T> Array2D<T> getrows(const Array2D<T> &A, int r0, int r1);
template <typename T> Array2D<T> getcols(const Array2D<T> &A, int c0, int c1);
template <typename T> Array2D<T> getrow(const Array2D<T> &A, int r);
template <typename T> Array2D<T> getcol(const Array2D<T> &A, int c);
template <typename T> void setsub(Array2D<T> &A, int r0, int r1, int c0, int c1, const Array2D<T> &B);
template <typename T> void setrows(Array2D<T> &A, int r0, int r1, const Array2D<T> &B);
template <typename T> void setcols(Array2D<T> &A, int c0, int c1, const Array2D<T> &B);
template <typename T> void setrow(Array2D<T> &A, int r, const Array2D<T> &B);
template <typename T> void setcol(Array2D<T> &A, int c, const Array2D<T> &B);

// manipulation functions
template <typename T> Array2D<T> transpose(const Array2D<T> &A);
template <typename T> Array2D<T> repmat(const Array2D<T> &A, unsigned int m, unsigned int n);
template <typename T> Array2D<T> reshape(const Array2D<T> &A, unsigned int m, unsigned int n);

// diag operators
template <typename T> Array2D<T> diag(const Array2D<T> &A);

// convert
template <typename T, typename S> void convert(Array2D<T> &B, const Array2D<S> &A);



//////////////////////////////////////////////////////////////////////////////
// template implementation
//////////////////////////////////////////////////////////////////////////////

template <typename T>
Array2D<T>::Array2D() : nrows(0), ncols(0)
{}

template <typename T>
Array2D<T>::Array2D(int nr, int nc) : nrows(nr), ncols(nc), data(nr*nc)
{
    assert(nr >= 0);
    assert(nc >= 0);
}

template <typename T>
Array2D<T>::Array2D(int nr, int nc, const T &a) : nrows(nr), ncols(nc), data(nr*nc, a)
{
    assert(nr >= 0);
    assert(nc >= 0);
}

template <typename T>
Array2D<T>::Array2D(int nr, int nc, const T *a) : nrows(nr), ncols(nc), data(nr*nc, a)
{
    assert(nr >= 0);
    assert(nc >= 0);
}

template <typename T>
Array2D<T>::Array2D(const Array2D &other): nrows(other.nrows), ncols(other.ncols), data(other.data)
{}

template <typename T>
Array2D<T>::~Array2D()
{}

template <typename T>
Array2D<T>& Array2D<T>::operator=(const Array2D &rhs)
{
    nrows = rhs.nrows;
    ncols = rhs.ncols;
    data = rhs.data;

    return *this;
}

template <typename T>
Array2D<T>& Array2D<T>::operator=(const T &a)
{
    data = a;

    return *this;
}

template <typename T>
Array2D<T>& Array2D<T>::set(const Array2D<bool> &flags, const T &a)
{
    assert(dim1() == flags.dim1());
    assert(dim2() == flags.dim2());

    data.set(flags.vector(), a);

    return *this;
}

template <typename T>
T& Array2D<T>::operator()(int i)
{
    return data[i];
}

template <typename T>
const T& Array2D<T>::operator()(int i) const
{
    return data[i];
}

template <typename T>
T& Array2D<T>::operator()(int i, int j)
{
#ifdef BOUNDS_CHECK
    assert((i < nrows) && (j < ncols));
#endif
    return data[i+nrows*j];
}

template <typename T>
const T& Array2D<T>::operator()(int i, int j) const
{
#ifdef BOUNDS_CHECK
    assert((i < nrows) && (j < ncols));
#endif
    return data[i+nrows*j];
}

template <typename T>
int Array2D<T>::dim1() const
{
    return nrows;
}

template <typename T>
int Array2D<T>::dim2() const
{
    return ncols;
}

template <typename T>
int Array2D<T>::size() const
{
    return data.size();
}

template <typename T>
void Array2D<T>::reshape(int nr, int nc)
{
    assert((nrows*ncols) == (nr*nc));

    nrows = nr;
    ncols = nc;
}

template <typename T>
const Array2D<T>& Array2D<T>::copy() const
{
    return *this;
}

template <typename T>
Array1D<T>& Array2D<T>::vector()
{
    return data;
}

template <typename T>
const Array1D<T>& Array2D<T>::vector() const
{
    return data;
}

template <typename T>
T* Array2D<T>::vals()
{
    return data.vals();
}

template <typename T>
T* Array2D<T>::vals() const
{
    return data.vals();
}



//////////////////////////////////////////////////////////////////////////////
// utility functions implementation
//////////////////////////////////////////////////////////////////////////////

// IO functions
template <typename T>
std::ostream& operator<<(std::ostream &s, const Array2D<T> &A)
{
    int M=A.dim1();
    int N=A.dim2();

    s << M << " " << N << "\n";
    for (int i=0; i<M; i++)
    {
        for (int j=0; j<N; j++)
            s << A(i, j) << " ";
        s << "\n";
    }

    return s;
}

template <typename T>
std::istream& operator>>(std::istream &s, Array2D<T> &A)
{
    int M, N;
    s >> M >> N;

    assert(M >= 0 && N >= 0);

    A = Array2D<T>(M,N);
    for (int i=0; i<M; i++)
        for (int j=0; j<N; j++)
            s >>  A(i, j);

    return s;
}

template <typename T>
void saveArray2D(const Array2D<T> &A, const std::string &filename)
{
    std::ofstream outfile;
    outfile.open(filename.c_str(), std::ostream::out);
    assert(outfile.is_open());

    outfile << A;
}

template <typename T>
Array2D<T> loadArray2D(const std::string &filename)
{
    std::ifstream infile;
    infile.open(filename.c_str(), std::ifstream::in);
    assert(infile.is_open());

    Array2D<T> A;
    infile >> A;

    return A;
}


// B = A(r0:r1, c0:c1)
template <typename T>
Array2D<T> getsub(const Array2D<T> &A, int r0, int r1, int c0, int c1)
{
    assert(r0 >= 0 && c0 >= 0);
    assert(r1 < A.dim1() && c1 < A.dim2());

    int m = r1-r0+1;
    int n = c1-c0+1;
    assert(m > 0 && n > 0);

    int indB = 0;
    int indA = r0+A.dim1()*c0;
    int colstep = r0+A.dim1()-r1-1;
    assert(colstep >= 0);

    Array2D<T> B(m, n);
//     // slower implementation but makes no assumption about storage
//     for(int i = 0; i < m; i++)
//         for(int j = 0; j < n; j++)
//             B(i, j) = A(i+r0, j+c0);
    // faster implementation but assumes column major storage
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < m; j++)
        {
            B(indB) = A(indA);
            ++indA;
            ++indB;
        }
        indA += colstep;
    }

    return B;
}

// B = A(r0:r1, :)
template <typename T>
Array2D<T> getrows(const Array2D<T> &A, int r0, int r1)
{
    return getsub(A, r0, r1, 0, A.dim2()-1);
}

// B = A(:, c0:c1)
template <typename T>
Array2D<T> getcols(const Array2D<T> &A, int c0, int c1)
{
    return getsub(A, 0, A.dim1()-1, c0, c1);
}

// B = A(r, :)
template <typename T>
Array2D<T> getrow(const Array2D<T> &A, int r)
{
    return getrows(A, r, r);
}

// B = A(:, c)
template <typename T>
Array2D<T> getcol(const Array2D<T> &A, int c)
{
    return getcols(A, c, c);
}

// A(r0:r1, c0:c1) = B
template <typename T>
void setsub(Array2D<T> &A, int r0, int r1, int c0, int c1, const Array2D<T> &B)
{
    assert(r0 >= 0 && c0 >= 0);
    assert(r1 < A.dim1() && c1 < A.dim2());

    int m = r1-r0+1;
    int n = c1-c0+1;
    assert(m > 0 && n > 0);
    assert(m == B.dim1() && n == B.dim2());

    int indB = 0;
    int indA = r0+A.dim1()*c0;
    int colstep = r0+A.dim1()-r1-1;
    assert(colstep >= 0);

//     // slower implementation but makes no assumption about storage
//     for(int i = 0; i < m; i++)
//         for(int j = 0; j < n; j++)
//             A(i+r0, j+c0) = B(i, j);
    // faster implementation but assumes column major storage
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < m; j++)
        {
            A(indA) = B(indB);
            ++indA;
            ++indB;
        }
        indA += colstep;
    }
}

// A(r0:r1, :) = B
template <typename T>
void setrows(Array2D<T> &A, int r0, int r1, const Array2D<T> &B)
{
    return setsub(A, r0, r1, 0, A.dim2()-1, B);
}

// A(:, c0:c1) = B
template <typename T>
void setcols(Array2D<T> &A, int c0, int c1, const Array2D<T> &B)
{
    return setsub(A, 0, A.dim1()-1, c0, c1, B);
}

// A(r, :) = B
template <typename T>
void setrow(Array2D<T> &A, int r, const Array2D<T> &B)
{
    return setrows(A, r, r, B);
}

// A(:, c) = B
template <typename T>
void setcol(Array2D<T> &A, int c, const Array2D<T> &B)
{
    return setcols(A, c, c, B);
}


// B = A'
template <typename T>
Array2D<T> transpose(const Array2D<T> &A)
{
    Array2D<T> B(A.dim2(), A.dim1());
    for(int i = 0; i < A.dim1(); i++)
        for(int j = 0; j < A.dim2(); j++)
            B(j, i) = A(i, j);

    return B;
}

// B = repmat(A, m, n);
template <typename T>
Array2D<T> repmat(const Array2D<T> &A, unsigned int m, unsigned int n)
{
    int nrows = A.dim1(), ncols = A.dim2();

    Array2D<T> B(nrows*m, ncols*n);
    for(unsigned int i = 0; i < m; i++)
        for(unsigned int j = 0; j < n; j++)
        {
            unsigned int row_start = i*nrows;
            unsigned int col_start = j*ncols;

            for(int k = 0; k < nrows; k++)
                for(int l = 0; l < ncols; l++)
                    B(row_start+k, col_start+l) = A(k, l);
        }

    return B;
}

// B = reshape(A, m, n)
template <typename T>
Array2D<T> reshape(const Array2D<T> &A, unsigned int m, unsigned int n)
{
    Array2D<double> B = A;
    B.reshape(m, n);

    return B;
}


// a = diag(A)
template <typename T>
Array2D<T> diag(const Array2D<T> &A)
{
    if(A.dim1() == 1 || A.dim2() == 1)
    {
        int n = A.size();

        Array2D<T> res(n, n, T(0));
        for(int i = 0; i < n; i++)
            res(i, i) = A(i);

        return res;
    }
    else
    {
        int mn = std::min(A.dim1(), A.dim2());

        Array2D<T> res(mn, 1);
        for(int i = 0; i < mn; i++)
            res(i) = A(i, i);

        return res;
    }
}


// convert
template <typename T, typename S> 
void convert(Array2D<T> &B, const Array2D<S> &A)
{
    assert(B.dim1() == A.dim1());
    assert(B.dim2() == A.dim2());

    for(int i = 0; i < A.size(); i++)
        B(i) = static_cast<T>(A(i));
}


}


#endif
