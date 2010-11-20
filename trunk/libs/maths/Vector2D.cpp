#include "Vector2D.h"

#include <cassert>

using std::ostringstream;

Vector2D::Vector2D()
{
    x = 0.0;
    y = 0.0;
}

Vector2D::Vector2D(double x, double y)
{
    this->x = x;
    this->y = y;
}

Vector2D::~Vector2D()
{}

double Vector2D::lengthSquared() const
{
    return x*x + y*y;
}

Vector2D Vector2D::operator+(const Vector2D &u) const
{
    return Vector2D(x+u.x, y+u.y);
}

Vector2D Vector2D::operator-(const Vector2D &u) const
{
    return Vector2D(x-u.x, y-u.y);
}

double& Vector2D::operator[](unsigned int selection)
{
    switch (selection)
    {
        case 0:
            return x;
            break;
        case 1:
            return y;
            break;
        default:
            assert(false);
    }
}

const double& Vector2D::operator[](unsigned int selection) const
{
    switch (selection)
    {
        case 0:
            return x;
            break;
        case 1:
            return y;
            break;
        default:
            assert(false);
    }
}

Vector2D operator*(const Vector2D &v, double u)
{
    Vector2D r;
    r[0] = v[0]*u;
    r[1] = v[1]*u;

    return r;
}

Vector2D operator/(const Vector2D &v, double u)
{
    return v*(1.0/u);
}

double operator*(const Vector2D &v, const Vector2D &u)
{
    return v[0]*u[0] + v[1]*u[1];
}

std::string Vector2D::toString() const
{
    ostringstream s;
    s << x << " " << y;

    return s.str();
}
