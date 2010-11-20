#ifndef ARRAY1D_H
#define ARRAY1D_H

#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdlib>


namespace TNT
{

template <typename T>
class Array1D
{
    public:
        Array1D();
        explicit Array1D(int n);
        Array1D(int n, const T &a);
        Array1D(int n, const T *a);
        Array1D(const Array1D &other);
        ~Array1D();

        inline Array1D& operator=(const Array1D &rhs);
        inline Array1D& operator=(const T &a);

        inline Array1D& set(const Array1D<bool> &flags, const T &a);

        inline T& operator[](int i);
        inline const T& operator[](int i) const;

        inline int dim() const;
        inline int size() const;

        inline const Array1D<T>& copy() const; // for compatibility with original TNT

        inline T* vals();
        inline T* vals() const;

    private:
        int nvals;
        T *the_vals;

        inline void allocateMemory(int n);
        inline void deallocateMemory();
        inline void resize(int n);
        inline void deepCopy(const Array1D &other);
};


// IO functions
template <typename T> std::ostream& operator<<(std::ostream &s, const Array1D<T> &a);
template <typename T> std::istream& operator>>(std::istream &s, Array1D<T> &a);
template <typename T> void saveArray1D(const Array1D<T> &a, const std::string &filename);
template <typename T> Array1D<T> loadArray1D(const std::string &filename);

// binary IO functions
template <typename T> void saveArray1D(const std::string &filename, const Array1D<T> &data);
template <typename T> void loadArray1D(const std::string &filename, Array1D<T> &data);

// sub vector functions
template <typename T> Array1D<T> getsub(const Array1D<T> &a, int v0, int v1);
template <typename T> void setsub(Array1D<T> &a, int v0, int v1, const Array1D<T> &b);



//////////////////////////////////////////////////////////////////////////////
// template implementation
//////////////////////////////////////////////////////////////////////////////

template <typename T>
Array1D<T>::Array1D() : nvals(0), the_vals(0)
{}

template <typename T>
Array1D<T>::Array1D(int n) : nvals(n), the_vals(0)
{
    allocateMemory(nvals);
}

template <typename T>
Array1D<T>::Array1D(int n, const T& a) : nvals(n), the_vals(0)
{
    allocateMemory(nvals);
    for(int i = 0; i < nvals; i++)
        the_vals[i] = a;
}

template <typename T>
Array1D<T>::Array1D(int n, const T *a) : nvals(n), the_vals(0)
{
    allocateMemory(nvals);
    for(int i = 0; i < nvals; i++)
        the_vals[i] = *a++;
}

template <typename T>
Array1D<T>::Array1D(const Array1D<T> &other) : nvals(other.nvals), the_vals(0)
{
    allocateMemory(nvals);
    deepCopy(other);
}

template <typename T>
Array1D<T>::~Array1D()
{
    deallocateMemory();
}

template <typename T>
Array1D<T>& Array1D<T>::operator=(const Array1D<T> &rhs)
{
    if(this != &rhs)
        deepCopy(rhs);

    return *this;
}

template <typename T>
Array1D<T>& Array1D<T>::operator=(const T &a)
{
    for(int i = 0; i < nvals; i++)
        the_vals[i] = a;

    return *this;
}

template <typename T>
Array1D<T>& Array1D<T>::set(const Array1D<bool> &flags, const T &a)
{
    assert(dim() == flags.dim());

    for(int i = 0; i < nvals; i++)
        if(flags[i])
            the_vals[i] = a;

    return *this;
}

template <typename T>
T& Array1D<T>::operator[](int i)
{
#ifdef BOUNDS_CHECK
    assert(i < nvals);
#endif
    return the_vals[i];
}

template <typename T>
const T& Array1D<T>::operator[](int i) const
{
#ifdef BOUNDS_CHECK
    assert(i < nvals);
#endif
    return the_vals[i];
}

template <typename T>
int Array1D<T>::dim() const
{
    return nvals;
}

template <typename T>
int Array1D<T>::size() const
{
    return nvals;
}

template <typename T>
const Array1D<T>& Array1D<T>::copy() const
{
    return *this;
}

template <typename T>
T* Array1D<T>::vals()
{
    return the_vals;
}

template <typename T>
T* Array1D<T>::vals() const
{
    return the_vals;
}

template <typename T>
void Array1D<T>::allocateMemory(int nv)
{
    assert(the_vals == 0);
    assert(nv >= 0);

    nvals = nv;

    if(nvals > 0) the_vals = new T[nvals];
}

template <typename T>
void Array1D<T>::deallocateMemory()
{
    if(the_vals != 0)
    {
        delete [] the_vals;
        the_vals = 0;
        nvals = 0;
    }
}

template <typename T>
void Array1D<T>::resize(int n)
{
    if(nvals != n)
    {
        deallocateMemory();
        allocateMemory(n);
    }
}

template <typename T>
void Array1D<T>::deepCopy(const Array1D &other)
{
    resize(other.nvals);
    for(int i = 0; i < nvals; i++)
        the_vals[i] = other.the_vals[i];
}



//////////////////////////////////////////////////////////////////////////////
// utility functions implementation
//////////////////////////////////////////////////////////////////////////////

template <typename T>
std::ostream& operator<<(std::ostream &s, const Array1D<T> &a)
{
    int N=a.dim();

    s << N << " ";
    for (int j=0; j<N; j++)
       s << a[j] << " ";
    s << "\n";

    return s;
}

template <typename T>
std::istream& operator>>(std::istream &s, Array1D<T> &a)
{
    int M;
    s >> M;

    assert(M >= 0);

    a = Array1D<T>(M);
    for (int i=0; i<M; i++)
        s >> a[i];

    return s;
}

template <typename T>
void saveArray1D(const Array1D<T> &a, const std::string &filename)
{
    std::ofstream outfile;
    outfile.open(filename.c_str(), std::ostream::out);
    assert(outfile.is_open());

    outfile << a;
}

template <typename T>
Array1D<T> loadArray1D(const std::string &filename)
{
    std::ifstream infile;
    infile.open(filename.c_str(), std::ifstream::in);
    assert(infile.is_open());

    Array1D<T> a;
    infile >> a;

    return a;
}


template <typename T>
void saveBinary(const std::string &filename, const Array1D<T> &data)
{
    std::ofstream out(filename.c_str(), std::ios::out|std::ios::binary);
    if(out.fail())
    {
        std::cout << "Failed to open file " << filename << std::endl;
        exit(1);
    }

    int nvals = data.size();
    out.write(reinterpret_cast<const char *>(&nvals), sizeof(int));
    out.write(reinterpret_cast<const char *>(data.vals()), sizeof(T)*nvals);

    out.close();
}

template <typename T>
void loadBinary(const std::string &filename, Array1D<T> &data)
{
    std::ifstream in(filename.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
    if(in.fail())
    {
        std::cout << "Failed to open file " << filename << std::endl;
        exit(1);
    }


    // get filesize from pointer at end of file
    const int intsize = sizeof(int);
    const int filesize = in.tellg();
    const int datasize = filesize-intsize;
    if(filesize < intsize)
    {
        std::cout << "Invalid binary file, only " << filesize << " bytes long." << std::endl;
        exit(1);
    }


    // allocate memory for data
    in.seekg(0, std::ios::beg);
    char nvals_pointer[intsize];
    in.read(nvals_pointer, intsize);
    int nvals = *(reinterpret_cast<int *>(nvals_pointer));


    // test size
    const int typesize = sizeof(T);
    if(nvals*typesize != datasize)
    {
        std::cout << "Incorrect file size." << std::endl;
        std::cout << "Expected " << nvals*typesize << " bytes = " << nvals << "*" << typesize << " bytes. Read " << datasize << " bytes." << std::endl;
        exit(1);
    }


    // read in data
    data = Array1D<T>(nvals);
    in.read(reinterpret_cast<char *>(data.vals()), datasize);
    in.close();
}


// b = a(v0:v1)
template <typename T>
Array1D<T> getsub(const Array1D<T> &a, int v0, int v1)
{
    assert(v0 >= 0);
    assert(v1 < a.dim());

    int m = v1-v0+1;

    assert(m > 0);

    Array1D<T> b(m);
    for(int i = 0; i < m; i++)
        b[i] = a[i+v0];

    return b;
}

// b(v0:v1) = a
template <typename T>
void setsub(Array1D<T> &a, int v0, int v1, const Array1D<T> &b)
{
    assert(v0 >= 0);
    assert(v1 < a.dim());

    int m = v1-v0+1;
    assert(m > 0);
    assert(m == b.dim());

    for(int i = 0; i < m; i++)
        a[i+v0] = b[i];
}


}


#endif
