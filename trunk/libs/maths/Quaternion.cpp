#include "Quaternion.h"

#include "Constants.h"

using MathConstants::EPSILON;
using MathConstants::PI;

#include <cassert>
#include <cmath>
#include <string>

using std::string;
using std::ostringstream;


double deg2rad(double degree)
{
    return degree * PI / 180.0;
}

double rad2deg(double radian)
{
    return radian * 180.0 / PI;
}


Quaternion::Quaternion()
{
    w = 1.0; x = 0.0; y = 0.0; z = 0.0;
}

Quaternion::Quaternion(double w, const Vector3D &v)
{
    this->w = w; this->x = v[0]; this->y = v[1]; this->z = v[2];
}

Quaternion::Quaternion(double w, double x, double y, double z)
{
    this->w = w; this->x = x; this->y = y; this->z = z;
}

Quaternion::~Quaternion()
{
}

double Quaternion::magnitude() const
{
    return (double)sqrt(w*w + x*x + y*y + z*z);
}

Vector3D Quaternion::getVector() const
{
    return Vector3D(x, y, z);
}

double Quaternion::getScalar() const
{
    return w;
}

void Quaternion::normalise()
{
    double m = magnitude();
    w /= m; x /= m; y /= m; z /= m;
}

Quaternion& Quaternion::operator+=(const Quaternion &q)
{
    w += q.w; x += q.x; y += q.y; z += q.z;

    return *this;
}

Quaternion& Quaternion::operator-=(const Quaternion &q)
{
    w -= q.w; x -= q.x; y -= q.y; z -= q.z;

    return *this;
}

Quaternion& Quaternion::operator*=(double s)
{
    w *= s; x *= s; y *= s; z *= s;

    return *this;
}

Quaternion& Quaternion::operator/=(double s)
{
    w /= s; x /= s; y /= s; z /= s;

    return *this;
}

Quaternion Quaternion::operator~() const
{
    return Quaternion(w, -x, -y, -z);
}

Quaternion Quaternion::operator+(const Quaternion &q) const
{
    return Quaternion(w + q.w, x + q.x, y + q.y, z + q.z);
}

Quaternion Quaternion::operator-(const Quaternion &q) const
{
    return Quaternion(w - q.w, x - q.x, y - q.y, z - q.z);
}

Quaternion Quaternion::operator*(const Quaternion &q) const
{
    return Quaternion(w*q.w - x*q.x - y*q.y - z*q.z,
                      w*q.x + x*q.w + y*q.z - z*q.y,
                      w*q.y + y*q.w + z*q.x - x*q.z,
                      w*q.z + z*q.w + x*q.y - y*q.x);
}

Quaternion Quaternion::operator/(double s) const
{
    return Quaternion(w/s, x/s, y/s, z/s);
}

Quaternion operator*(const Quaternion &q, double s)
{
    return Quaternion(q.w*s, q.x*s, q.y*s, q.z*s);
}

Quaternion operator*(double s, const Quaternion &q)
{
    return Quaternion(q.w*s, q.x*s, q.y*s, q.z*s);
}

Quaternion operator*(const Quaternion &q, const Vector3D &v)
{
    double qw = q.w, qx = q.x, qy = q.y, qz = q.z;
    double vx = v[0], vy = v[1], vz = v[2];

    return Quaternion(-qx*vx - qy*vy - qz*vz,
                       qw*vx + qy*vz - qz*vy,
                       qw*vy + qz*vx - qx*vz,
                       qw*vz + qx*vy - qy*vx);
}

Quaternion operator*(const Vector3D &v, const Quaternion &q)
{
    double qw = q.w, qx = q.x, qy = q.y, qz = q.z;
    double vx = v[0], vy = v[1], vz = v[2];

    return Quaternion(-vx*qx - vy*qy - vz*qz,
                       vx*qw + vy*qz - vz*qy,
                       vy*qw + vz*qx - vx*qz,
                       vz*qw + vx*qy - vy*qx);
}

double Quaternion::angle() const
{
    return (2.0 * acos(w));
}

Vector3D Quaternion::axis() const
{
    Vector3D myaxis(x, y, z);
    myaxis.normalise();

    if(myaxis.magnitude() < EPSILON)
        myaxis = Vector3D(1.0, 0.0, 0.0);

    return myaxis;
}

Quaternion Quaternion::rotate(const Quaternion &q) const
{
    return (*this)*q*(~(*this));
}

Vector3D Quaternion::rotate(const Vector3D &v) const
{
    Quaternion t;
    t = (*this)*v*(~(*this));

    return Vector3D(t.x, t.y, t.z);
}

Quaternion Quaternion::makeFromAxisAngle(const Vector3D &axis, double angle)
{
    double result = sin(0.5 * angle);

    if ((fabs(axis[0]) < EPSILON) && (fabs(axis[1]) < EPSILON) && (fabs(axis[2]) < EPSILON))
        return Quaternion(1.0, 0.0, 0.0, 0.0);

    Vector3D at = axis * result;

    Quaternion q(cos(0.5 * angle), at);
    q.normalise();

    return q;
}

Quaternion Quaternion::makeFromEulerAngles(double roll, double pitch, double yaw)
{
    double cyaw = cos(0.5 * yaw), cpitch = cos(0.5 * pitch), croll = cos(0.5 * roll);
    double syaw = sin(0.5 * yaw), spitch = sin(0.5 * pitch), sroll = sin(0.5 * roll);
    double cyawcpitch = cyaw * cpitch, syawspitch = syaw * spitch;
    double cyawspitch = cyaw * spitch, syawcpitch = syaw * cpitch;

    double w = cyawcpitch * croll + syawspitch * sroll;
    double x = cyawcpitch * sroll - syawspitch * croll;
    double y = cyawspitch * croll + syawcpitch * sroll;
    double z = syawcpitch * croll - cyawspitch * sroll;

    Quaternion q(w, x, y, z);
    q.normalise();

    return q;
}

//output in radian
Vector3D Quaternion::makeEulerAngles() const
{
    Vector3D u;

    double q00 = w*w, q11 = x*x, q22 = y*y, q33 = z*z;

    double r11 = q00 + q11 - q22 - q33;
    double r21 = 2.0 * (x*y + w*z);
    double r31 = 2.0 * (x*z - w*y);
    double r32 = 2.0 * (y*z + w*x);
    double r33 = q00 - q11 - q22 + q33;

    double r12, r13;
    double temp = fabs(r31);
    if (temp > 0.999999)
    {
        r12 = 2.0 * (x*y - w*z);
        r13 = 2.0 * (x*z + w*y);

        u[0] = 0.0;                    //roll
        u[1] = -(PI / 2.0) * r31 / temp;     //pitch
        u[2] = atan2(-r12, -r31 * r13);//yaw
    }
    else
    {
        u[0] = atan2(r32, r33);  //roll
        u[1] = asin(-r31);       //pitch
        u[2] = atan2(r21, r11);  //yaw
    }

    return u;
}

double& Quaternion::operator[](unsigned int selection)
{
    switch (selection)
    {
        case 0:
            return w;
            break;
        case 1:
            return x;
            break;
        case 2:
            return y;
            break;
        case 3:
            return z;
            break;
        default:
            assert(false);
            return w;
    }
}

const double& Quaternion::operator[](unsigned int selection) const
{
    switch (selection)
    {
        case 0:
            return w;
            break;
        case 1:
            return x;
            break;
        case 2:
            return y;
            break;
        case 3:
            return z;
            break;
        default:
            assert(false);
            return w;
    }
}

//Spherical linear interpolation, both Quaternions will have to be absolute in
//in the global coordinate space system, not relative
//based on code from gamasutra.com
//UNTESTED
Quaternion Quaternion::slerp(const Quaternion &from, const Quaternion &to, double t)
{
    double tol[4];
    double cosom = from.w * to.w + from.x * to.x + from.y * to.y + from.z * to.z;
    if (cosom < 0.0)
    {
        cosom = -cosom;
        tol[0] = -to.w; tol[1] = -to.x; tol[2] = -to.y; tol[3] = -to.z;
    }
    else
    {
        tol[0] = to.w;  tol[1] = to.x;  tol[2] = to.y;  tol[3] = to.z;
    }

    //calculate coefficient
    double scale0, scale1;
    if ((1.0 - cosom) > 0.000001)
    {
        double omega = acos(cosom);
        double sinom = sin(omega);
        scale0 = (sin(1.0 - t) * omega) / sinom;
        scale1 = sin(t * omega) / sinom;
    }
    else//from and to are very close
    {
        scale0 = 1.0 - t;
        scale1 = t;
    }

    Quaternion result = Quaternion( scale0 * from.w + scale1 * tol[0],
                                    scale0 * from.x + scale1 * tol[1],
                                    scale0 * from.y + scale1 * tol[2],
                                    scale0 * from.z + scale1 * tol[3]);
    result.normalise();

    return result;
}

string Quaternion::toString() const
{
    ostringstream s;
    s << w << " " << x << " " << y << " " << z;

    return s.str();
}

double computeAlignment(const Quaternion &source_quat, const Quaternion &target_quat, const Vector3D &rotation_axis)
{
    Quaternion diff_quat = target_quat * ~source_quat;
    double p = diff_quat.getVector() * rotation_axis;
    double n = diff_quat.getScalar();
    double theta = 2.0*atan2(p, n);

    return theta;
}
