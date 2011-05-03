#include "CrossPlatform.h"
#include "TimeFuncs.h"

#include "tnt/array2d.h"
#include "tnt/jama_chol.h"
#include "tnt/jama_eig.h"
#include "tnt/jama_qr.h"
#include "tnt/jama_svd.h"
#include "tnt/jama_svd_funcs.h"
#include "tnt/jama_lu.h"
#include "tnt/jama_lu_funcs.h"

#include <iostream>
#include <cstdlib>
#include <string>

using namespace TNT;
using namespace std;

int main(int argc, char *argv[])
{
//     Array2D<double> bah1 = loadArray2D<double>("bah1.mat");
//     JAMA::LU<double> lu1(bah1);
//     std::cout << lu1.getL() << std::endl;
//     std::cout << lu1.getU() << std::endl;
//
//     Array2D<double> bah2 = loadArray2D<double>("bah2.mat");
//     JAMA::LU<double> lu2(bah2);
//     std::cout << lu2.getL() << std::endl;
//     std::cout << lu2.getU() << std::endl;
//
//     std::cout << cov(bah1) << std::endl;
//
//     std::cout << reshape(bah1, 2, 18) << std::endl;
//
//     Array2D<double> bah = loadArray2D<double>("bah1.mat");
//     std::cout << getsub(bah, 2, 4, 1, 4) << std::endl;
//     Array2D<double> gah(2, 3);
//     gah(0, 0) = 1; gah(0, 1) = 2; gah(0, 2) = 3;
//     gah(1, 0) = 2; gah(1, 1) = 4; gah(1, 2) = 6;
//     setsub(bah, 2, 3, 1, 3, gah);
//     std::cout << bah << std::endl;
//
//
//     Array2D<double> rah(2000, 1500);
//     for(int i = 0; i < rah.dim1(); i++)
//         for(int j = 0; j < rah.dim2(); j++)
//             rah(i, j) = rand();
//     double rah0 = getTime();
//     Array2D<double> subrah = getsub(rah, 101, 1500, 201, 1200);
//     double rah1 = getTime();
//     std::cout << "elapsed time = " << rah1 - rah0 << std::endl;
//
//     return 0;


    //srand(getSeedFromTime());

    unsigned int iters = 500000;

    unsigned int numVals = 8;
    Array1D<double> x(numVals), y(numVals), z;
    for(int i = 0; i < x.dim(); i++)
    {
        x[i] = static_cast<double>(rand())/RAND_MAX;
        y[i] = static_cast<double>(rand())/RAND_MAX;
    }
    cout << "New x:" << endl;
    for(int i = 0; i < x.dim(); i++)
        cout << x[i] << " ";
    cout << endl;
    cout << "New y:" << endl;
    for(int i = 0; i < y.dim(); i++)
        cout << y[i] << " ";
    cout << endl << endl;

    cout << "test Array1D<double> operator-(const Array1D<double> &rhs) " << getSeedFromTime() << endl;
    z = -x;
    for(int i = 0; i < y.dim(); i++)
        cout << z[i] << " ";
    cout << endl;
    double beforeTime = getTime();
    for(unsigned int i = 0; i < iters; i++)
        z = -x;
    double afterTime = getTime();
    cout << "time elapsed = " << afterTime - beforeTime << endl;
    cout << endl;

    double d;
    cout << "test double dot(const Array1D<double> &lhs, const Array1D<double> &rhs) " << getSeedFromTime() << endl;
    d = dot(x, y);
    cout << d << endl;
    beforeTime = getTime();
    for(unsigned int i = 0; i < iters; i++)
      d = dot(x, y);
    afterTime = getTime();
    cout << "time elapsed = " << afterTime - beforeTime << endl;
    cout << endl;

    cout << "test Array1D<double> operator+(const Array1D<double> &lhs, const Array1D<double> &rhs) " << getSeedFromTime() << endl;
    z = x+y;
    for(int i = 0; i < y.dim(); i++)
        cout << z[i] << " ";
    cout << endl;
    beforeTime = getTime();
    for(unsigned int i = 0; i < iters; i++)
        z = x+y;
    afterTime = getTime();
    cout << "time elapsed = " << afterTime - beforeTime << endl;
    cout << endl;

    cout << "test Array1D<double> operator-(const Array1D<double> &lhs, const Array1D<double> &rhs) " << getSeedFromTime() << endl;
    z = x-y;
    for(int i = 0; i < y.dim(); i++)
        cout << z[i] << " ";
    cout << endl;
    beforeTime = getTime();
    for(unsigned int i = 0; i < iters; i++)
        z = x-y;
    afterTime = getTime();
    cout << "time elapsed = " << afterTime - beforeTime << endl;
    cout << endl;

    cout << "test Array1D<double> operator*(const Array1D<double> &lhs, double rhs) " << getSeedFromTime() << endl;
    z = x*3.4;
    for(int i = 0; i < y.dim(); i++)
        cout << z[i] << " ";
    cout << endl;
    beforeTime = getTime();
    for(unsigned int i = 0; i < iters; i++)
        z = x*3.4;
    afterTime = getTime();
    cout << "time elapsed = " << afterTime - beforeTime << endl;
    cout << endl;

    cout << "Array1D<double> operator*(double lhs, const Array1D<double> &rhs); " << getSeedFromTime() << endl;
    z = -5.6*y;
    for(int i = 0; i < y.dim(); i++)
        cout << z[i] << " ";
    cout << endl;
    beforeTime = getTime();
    for(unsigned int i = 0; i < iters; i++)
        z = -5.6*y;
    afterTime = getTime();
    cout << "time elapsed = " << afterTime - beforeTime << endl;
    cout << endl;

    unsigned int nrow = 10, ncol = 8;
    Array2D<double> A(nrow, ncol), B(ncol, nrow), C, D(nrow, ncol);
    for(unsigned int i = 0; i < nrow; i++)
        for(unsigned int j  =0; j < ncol; j++)
        {
            A(i, j) = static_cast<double>(rand())/RAND_MAX;
            B(j, i) = static_cast<double>(rand())/RAND_MAX;
            D(i, j) = static_cast<double>(rand())/RAND_MAX;
        }

    cout << "A:" << endl;
    for(unsigned int i = 0; i < nrow; i++)
    {
        for(unsigned int j  =0; j < ncol; j++)
            cout << A(i, j) << " ";
        cout << endl;
    }
    cout << endl;

    cout << "B:" << endl;
    for(unsigned int i = 0; i < ncol; i++)
    {
        for(unsigned int j  =0; j < nrow; j++)
            cout << B(i, j) << " ";
        cout << endl;
    }
    cout << endl;

    cout << "D:" << endl;
    for(unsigned int i = 0; i < nrow; i++)
    {
        for(unsigned int j  =0; j < ncol; j++)
            cout << D(i, j) << " ";
        cout << endl;
    }
    cout << endl;

    cout << "test Array2D<double> operator-(const Array2D<double> &rhs); " << getSeedFromTime() << endl;
    C = -A;
    for(int i = 0; i < C.dim1(); i++)
    {
        for(int j = 0; j < C.dim2(); j++)
            cout << C(i, j) << " ";
        cout << endl;
    }
    beforeTime = getTime();
    for(unsigned int i = 0; i < iters; i++)
        C = -A;
    afterTime = getTime();
    cout << "time elapsed = " << afterTime - beforeTime << endl;
    cout << endl;

    cout << "test Array2D<double> operator*(const Array2D<double> &lhs, const Array2D<double> &rhs); " << getSeedFromTime() << endl;
    C = A*B;
    for(int i = 0; i < C.dim1(); i++)
    {
        for(int j = 0; j < C.dim2(); j++)
            cout << C(i, j) << " ";
        cout << endl;
    }
    beforeTime = getTime();
    for(unsigned int i = 0; i < iters; i++)
        C = A*B;
    afterTime = getTime();
    cout << "time elapsed = " << afterTime - beforeTime << endl;
    cout << endl;

    cout << "test Array2D<double> operator+(const Array2D<double> &lhs, const Array2D<double> &rhs) " << getSeedFromTime() << endl;
    C = A+D;
    for(int i = 0; i < C.dim1(); i++)
    {
        for(int j = 0; j < C.dim2(); j++)
            cout << C(i, j) << " ";
        cout << endl;
    }
    beforeTime = getTime();
    for(unsigned int i = 0; i < iters; i++)
        C = A+D;
    afterTime = getTime();
    cout << "time elapsed = " << afterTime - beforeTime << endl;
    cout << endl;

    cout << "test Array2D<double> operator-(const Array2D<double> &lhs, const Array2D<double> &rhs) " << getSeedFromTime() << endl;
    C = A-D;
    for(int i = 0; i < C.dim1(); i++)
    {
        for(int j = 0; j < C.dim2(); j++)
            cout << C(i, j) << " ";
        cout << endl;
    }
    beforeTime = getTime();
    for(unsigned int i = 0; i < iters; i++)
        C = A-D;
    afterTime = getTime();
    cout << "time elapsed = " << afterTime - beforeTime << endl;
    cout << endl;

    cout << "test Array2D<double> operator*(const Array2D<double> &lhs, double multiplier) " << getSeedFromTime() << endl;
    C = A*d;
    for(int i = 0; i < C.dim1(); i++)
    {
        for(int j = 0; j < C.dim2(); j++)
            cout << C(i, j) << " ";
        cout << endl;
    }
    beforeTime = getTime();
    for(unsigned int i = 0; i < iters; i++)
        C = A*d;
    afterTime = getTime();
    cout << "time elapsed = " << afterTime - beforeTime << endl;
    cout << endl;

    cout << "Array2D<double> operator*(double multiplier, const Array2D<double> &rhs) " << getSeedFromTime() << endl;
    C = -d*B;
    for(int i = 0; i < C.dim1(); i++)
    {
        for(int j = 0; j < C.dim2(); j++)
            cout << C(i, j) << " ";
        cout << endl;
    }
    beforeTime = getTime();
    for(unsigned int i = 0; i < iters; i++)
        C = -d*A;
    afterTime = getTime();
    cout << "time elapsed = " << afterTime - beforeTime << endl;
    cout << endl;

    cout << "test Array2D<double> operator/(const Array2D<double> &lhs, double denominator); " << getSeedFromTime() << endl;
    C = A/d;
    for(int i = 0; i < C.dim1(); i++)
    {
        for(int j = 0; j < C.dim2(); j++)
            cout << C(i, j) << " ";
        cout << endl;
    }
    beforeTime = getTime();
    for(unsigned int i = 0; i < iters; i++)
        C = A/d;
    afterTime = getTime();
    cout << "time elapsed = " << afterTime - beforeTime << endl;
    cout << endl;

    cout << "test Array1D<double> operator*(const Array2D<double> &lhs, const Array1D<double> &rhs); " << getSeedFromTime() << endl;
    y = A*x;
    for(int i = 0; i < y.dim(); i++)
        cout << y[i] << " ";
    cout << endl;
    beforeTime = getTime();
    for(unsigned int i = 0; i < iters; i++)
        y=A*x;
    afterTime = getTime();
    cout << "time elapsed = " << afterTime - beforeTime << endl;
    cout << endl;

    cout << "test Array1D<double> operator*(const Array1D<double> &lhs, const Array2D<double> &rhs) " << getSeedFromTime() << endl;
    y = x*B;
    for(int i = 0; i < y.dim(); i++)
        cout << y[i] << " ";
    cout << endl;
    beforeTime = getTime();
    for(unsigned int i = 0; i < iters; i++)
        y=x*B;
    afterTime = getTime();
    cout << "time elapsed = " << afterTime - beforeTime << endl;
    cout << endl;

    Array2D<double> E(nrow-3, ncol);
    for(unsigned int i = 0; i < nrow-3; i++)
        for(unsigned int j  =0; j < ncol; j++)
            E(i, j) = static_cast<double>(rand())/RAND_MAX;
    cout << "E:" << endl;
    for(unsigned int i = 0; i < nrow-3; i++)
    {
        for(unsigned int j  =0; j < ncol; j++)
            cout << D(i, j) << " ";
        cout << endl;
    }
    cout << endl;

    cout << "test Array2D<double> dist2(const Array2D<double> &m, const Array2D<double> &n); " << getSeedFromTime() << endl;
    C = dist2(A, E);
    for(int i = 0; i < C.dim1(); i++)
    {
        for(int j = 0; j < C.dim2(); j++)
            cout << C(i, j) << " ";
        cout << endl;
    }
    beforeTime = getTime();
    for(unsigned int i = 0; i < iters; i++)
        C = dist2(A, E);
    afterTime = getTime();
    cout << "time elapsed = " << afterTime - beforeTime << endl;
    cout << endl;

    cout << "test Matrix inversion " << getSeedFromTime() << endl;
    Array2D<double> before_matrix(10, 10);
    for(int i = 0; i < before_matrix.dim1(); i++)
        for(int j  =0; j < before_matrix.dim2(); j++)
            before_matrix(i, j) = static_cast<double>(rand())/RAND_MAX;

    for(int i = 0; i < before_matrix.dim1(); i++)
    {
        for(int j  =0; j < before_matrix.dim2(); j++)
            cout << before_matrix(i, j) << " ";
        cout << endl;
    }
    std::cout << endl;

    double det = -1.0;
    Array2D<double> after_matrix = inv(before_matrix, det);
    std::cout << "det = " << det << std::endl;
    for(int i = 0; i < after_matrix.dim1(); i++)
    {
        for(int j  =0; j < after_matrix.dim2(); j++)
            cout << after_matrix(i, j) << " ";
        cout << endl;
    }
    std::cout << endl;

    Array2D<double> should_be_eye = before_matrix*after_matrix;
    for(int i = 0; i < should_be_eye.dim1(); i++)
    {
        for(int j  =0; j < should_be_eye.dim2(); j++)
            cout << should_be_eye(i, j) << " ";
        cout << endl;
    }
    std::cout << endl;

    cout << "Finished" << endl;


    for(unsigned int k = 0; k < iters/1000; k++)
    {
        Array2D<double> mat1(10, 10);
        for(int i = 0; i < mat1.dim1(); i++)
            for(int j = 0; j < mat1.dim2(); j++)
                mat1(i, j) = rand();
        Array2D<double> mat11 = eye<double>(3);
        for(int i = 0; i < mat11.dim1(); i++)
            mat11(i, i) = rand();

        Array2D<double> mat2(10, 20);
        for(int i = 0; i < mat2.dim1(); i++)
            for(int j = 0; j < mat2.dim2(); j++)
                mat2(i, j) = rand();
        Array2D<double> mat3(20, 10);
        for(int i = 0; i < mat3.dim1(); i++)
            for(int j = 0; j < mat3.dim2(); j++)
                mat3(i, j) = rand();

        JAMA::QR<double> qr(mat1);
        JAMA::Cholesky<double> chol(mat1);
        JAMA::Eigenvalue<double> eig(mat11);
        JAMA::LU<double> lu(mat1);
        JAMA::SVD<double> svd1(mat2);
        JAMA::SVD<double> svd2(mat3);

        Array2D<double> mat4 = inv(mat1);
        mat4 = pinv(mat2);
        mat4 = pinv(mat3);

//         std::cout << mat3 << std::endl;
//         std::cout << mat4 << std::endl;
//         std::cout << mat4*mat3 << std::endl;
//         exit(1);
    }

    return 0;
}
