#include "Quadric.h"

using std::string;

Quadric::Quadric()
{
    m_Q = zeros<double>(4);
}

Quadric::Quadric(const Array2D<double> &Q, const string &type)
: m_Q(Q.copy()), m_type(type)
{
    assert(m_Q.dim1() == 4);
    assert(m_Q.dim2() == 4);
}

Quadric::~Quadric()
{}

Quadric Quadric::createEllipsoid(const double &width, const double &height, const double &depth)
{
    Array2D<double> Q(4, 4, 0.0);
    Q(0, 0) = 1.0/(width*width); Q(1, 1) = 1.0/(height*height); Q(2, 2) = 1.0/(depth*depth); Q(3, 3) = -1.0;
    return Quadric(Q, "ellipsoid");
}

const Array2D<double>& Quadric::Q() const
{
    return m_Q;
}

const std::string& Quadric::type() const
{
    return m_type;
}

Quadric Quadric::transform(const Array2D<double> &H) const
{
    return Quadric(transpose(H)*Q()*H, type());
}

Conic Quadric::computeConic() const
{
    Array2D<double> A = getsub(Q(), 0, 2, 0, 2);
    Array2D<double> b = getsub(Q(), 0, 2, 3, 3);
    double c = Q()(3, 3);

    return Conic(c*A - b*transpose(b));
}

Conic Quadric::computeConic(const Array2D<double> &H) const
{
    return transform(H).computeConic();
}
