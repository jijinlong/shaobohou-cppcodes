#include "Conic.h"

Conic::Conic()
{
    m_C = zeros<double>(3);
}

Conic::Conic(const Array2D<double> &C)
: m_C(C.copy())
{
    assert(m_C.dim1() == 3);
    assert(m_C.dim2() == 3);
}

Conic::~Conic()
{}

const Array2D<double>& Conic::C() const
{
    return m_C;
}

Conic Conic::transform(const Array2D<double> &H) const
{
    return Conic(transpose(H)*C()*H);
}

Array1D<double> Conic::computeRadii() const
{
    JAMA::Eigenvalue<double> eig(C());
    Array2D<double> V = eig.getV();
    Array2D<double> D = eig.getD();

    Array1D<double> radii(2);
    double a = sqrt(-D(0, 0)/D(1, 1));
    double b = sqrt(-D(0, 0)/D(2, 2));

    radii[0] = a; radii[1] = b;

    return radii;
}
