#ifndef MATRIX_ALGORITHMS_H
#define MATRIX_ALGORITHMS_H

template <typename T>
NRMatrix<T> solveBlockTridiagonal(const NRMatrix<T> &A, const NRMatrix<T> &B)
{
    unsigned int nrows = A.numRows(), ncols = A.numCols();
    assert((nrows > 0) && (ncols > 0));
    assert(nrows == B.numRows());
    assert((ncols % 3) == 0);
    unsigned int dim = ncols / 3;
    assert((nrows % dim) == 0);
    unsigned int num_blocks = nrows / dim;

    // get diagonal and off diagonal block
    NRMatrix<T> F(A(dim, nrows-1, 0, dim-1));           // lower off diagonal
    NRMatrix<T> D(A(0, nrows-1, dim, dim*2-1));         // diagonal
    NRMatrix<T> E(A(0, nrows-dim-1, dim*2, dim*3-1));   // upper off diagonal

    // compute LU block decompositions
    NRMatrix<T> L(nrows-dim, dim), U(nrows, dim);
    U.rows(0, dim, D.rows(0, dim));
    for(unsigned int i = 1; i < num_blocks; i++)
    {
        unsigned int row_start = (i-1)*dim, row_end = i*dim-1;
        L.rows(row_start,     row_end,     mldivide(U.rows(row_start, row_end), E.rows(row_start, row_end)));
        U.rows(row_start+dim, row_end+dim, D.rows(row_start+dim, row_end+dim)-F.rows(row_start, row_end)*L.rows(row_start, row_end));
    }

    // solve for Y
    NRMatrix<T> Y(B.numRows(), B.numCols());
    Y.rows(0, dim-1, mldivide(U.rows(0, dim-1), B.rows(0, dim-1)));
    for(unsigned int i = 1; i < num_blocks; i++)
    {
        unsigned int row_start = (i-1)*dim, row_end = i*dim-1;
        Y.rows(row_start+dim, row_end+dim, mldivide(U.rows(row_start+dim, row_end+dim), B.rows(row_start+dim, row_end+dim)-F.rows(row_start, row_end)*Y.rows(row_start, row_end)));
    }

    // solve for X
    NRMatrix<T> X(B.numRows(), B.numCols());
    X.rows(nrows-dim, nrows-1, Y.rows(nrows-dim, nrows-1));
    for(unsigned int i = num_blocks-1; i > 0; i--)
    {
        unsigned int row_start = (i-1)*dim, row_end = i*dim-1;
        X.rows(row_start, row_end, Y.rows(row_start, row_end)-L.rows(row_start, row_end)*X.rows(row_start+dim, row_end+dim));
    }

    return X;
}

#endif
