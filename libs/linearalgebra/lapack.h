/*

lapack.h

BLAS and LAPACK header file

Shaobo Hou
12/12/2006

*/

#ifndef LAPACK_H
#define LAPACK_H

// ***** BLAS Level 2 operations *****

// y:= alpha*op(A)*x + beta*y
extern "C" void dgemv_(const char *trans, const int &m, const int &n,
               const double &alpha, const double *A, const int &lda, const double *x, const int &incx,
               const double &beta, const double *y, const int &incy);


// ***** BLAS Level 3 operations *****

// C:= alpha*op(A)*op(B) + beta*C
extern "C" void dgemm_(const char *transa, const char *transb, const int &m, const int &n, const int &k,
                       const double &alpha, const double *A, const int &lda, const double *B, const int &ldb,
                       const double &beta, double *C, const int &ldc);


// ***** LAPACK operations *****

// Compute an LU factorization of a general M by N matrix A.
extern "C" void dgetrf_(const int &m, const int &n, const double *A, const int &lda, int *ipiv, int &info);

// Compute the inverse using the LU factorization computed by dgetrf.
extern "C" void dgetri_(const int &n, const double *A, const int &lda, const int *ipiv, double *work, const int &lwork, int &info);

// Compute computes the solution to a real system of linear equations A * X = B
extern "C" void dgesv_(const int &n, const int &nrhs, const double *A, const int &lda, const int *ipiv, const double *B, const int &ldb, int &info);

// Compute a cholesky factorization of a positive definite symmetric N by N matrix A.
extern "C" void dpotrf_(const char *uplo, const int &n, const double *A, const int &lda, int &info);

// Compute the inverse using the cholesky factorization computed by dpotrf.
extern "C" void dpotri_(const char *uplo, const int &n, const double *A, const int &lda, int &info);

// Compute computes the solution to a symmetric positive definite system of linear equations A * X = B
extern "C" void dpotrs_(const char *uplo, const int &n, const int &nrhs, const double *A, const int &lda, const double *B, const int &ldb, int &info);

// Compute computes the solution to a upper or lower triangular system of linear equations A * X = B
extern "C" void dtrsm_(const char *side, const char *uplo, const char *trans, const char *diag, const int &m, const int &n,
                const double &alpha, const double *A, const int &lda, const double *B, const int &ldb);


extern "C" void dgeqp3_(const int &m, const int &n, const double *A, const int &lda, const int *ipiv, const double *tau, double *work, const int &lwork, int &info);

#endif
