#ifndef CONIC_H
#define CONIC_H

#include "LinearAlgebra.h"

class Conic
{
    public:
        Conic();
        Conic(const Array2D<double> &C);
        virtual ~Conic();

        const Array2D<double>& C() const;

        Conic transform(const Array2D<double> &H) const;

        Array1D<double> computeRadii() const;

    private:
        Array2D<double> m_C;
};

#endif
