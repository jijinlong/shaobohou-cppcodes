#include "Vector3D.h"

#include <cassert>
#include <cmath>
#include <string>

using std::string;
using std::ostringstream;


const double EPSILON = 1.1921e-007;


Vector3D::Vector3D()
{
    x = 0.0; y = 0.0; z = 0.0;
}

Vector3D::Vector3D(double x, double y, double z)
{
    this->x = x; this->y = y; this->z = z;
}

Vector3D::~Vector3D()
{
}

double Vector3D::magnitude() const
{
    return static_cast<double>(sqrt(x*x + y*y + z*z));
}

Vector3D Vector3D::direction() const
{
    Vector3D dir(x, y, z);
    dir.normalise();

    return dir;
}

void Vector3D::normalise()
{
    double m = magnitude();

    if (m < EPSILON) m = 1.0;

    x /= m;
    y /= m;
    z /= m;

    if (fabs(x) < EPSILON) x = 0.0;
    if (fabs(y) < EPSILON) y = 0.0;
    if (fabs(z) < EPSILON) z = 0.0;
}

Vector3D& Vector3D::operator+=(const Vector3D &u)
{
    x += u.x; y += u.y; z += u.z;

    return *this;
}

Vector3D& Vector3D::operator-=(const Vector3D &u)
{
    x -= u.x; y -= u.y; z -= u.z;

    return *this;
}

Vector3D& Vector3D::operator*=(double s)
{
    x *= s; y *= s; z *= s;

    return *this;
}

Vector3D& Vector3D::operator/=(double s)
{
    x /= s; y /= s; z /= s;

    return *this;
}

Vector3D Vector3D::operator-() const
{
    return Vector3D(-x, -y, -z);
}

Vector3D Vector3D::operator+(const Vector3D &u) const
{
    return Vector3D(x + u.x, y + u.y, z + u.z);
}

Vector3D Vector3D::operator-(const Vector3D &u) const
{
    return Vector3D(x - u.x, y - u.y, z - u.z);
}

Vector3D Vector3D::operator^(const Vector3D &u) const
{
    return Vector3D(y*u.z - z*u.y, z*u.x - x*u.z, x*u.y - y*u.x);
}

double Vector3D::operator*(const Vector3D &u) const
{
    return (x*u.x + y*u.y + z*u.z);
}

Vector3D Vector3D::operator/(double s) const
{
    return Vector3D(x / s, y / s, z / s);
}

Vector3D operator*(const Vector3D &u, double s)
{
    return Vector3D(u.x * s, u.y * s, u.z *s);
}

Vector3D operator*(double s, const Vector3D &u)
{
    return Vector3D(u.x * s, u.y * s, u.z *s);
}

Vector3D Vector3D::normal(const Vector3D &v1, const Vector3D &v2, const Vector3D &v3)
{
    Vector3D line1 = (v1 - v2);
    Vector3D line2 = (v2 - v3);

    Vector3D normal = line1 ^ line2;
    normal.normalise();
    return normal;
}

double Vector3D::angle(const Vector3D &v1, const Vector3D &v2)
{
    Vector3D a = v1, b = v2;
    a.normalise();
    b.normalise();
    double angle = acos(a * b);

    return angle;
}

double Vector3D::distance(const Vector3D &v1, const Vector3D &v2)
{
    Vector3D vt = v2 - v1;
    return sqrt(vt * vt);
}

Vector3D Vector3D::getFloatVersion(const Vector3D &doubleVersion)
{
    return Vector3D((float)doubleVersion.x, (float)doubleVersion.y, (float)doubleVersion.z);
}

double& Vector3D::operator[](unsigned int selection)
{
    switch (selection)
    {
        case 0:
            return x;
            break;
        case 1:
            return y;
            break;
        case 2:
            return z;
            break;
        default:
            assert(false);
            return x;
    }
}

const double& Vector3D::operator[](unsigned int selection) const
{
    switch (selection)
    {
        case 0:
            return x;
            break;
        case 1:
            return y;
            break;
        case 2:
            return z;
            break;
        default:
            assert(false);
            return x;
    }
}

string Vector3D::toString() const
{
    ostringstream s;
    s << x << " " << y << " " << z;

    return s.str();
}
