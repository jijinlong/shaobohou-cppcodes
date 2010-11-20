#include "ExponentialMap.h"

#include "special.h"

const double minAngle = 1.0e-7;
const double cutoffAngle = pi;

Quaternion exp(const Vector3D &v)
{
    double theta = sqrt(v * v);
    double cosp = cos(0.5 * theta);
    double sinp = sin(0.5 * theta);
    Vector3D axis;

    if (theta < minAngle)
        axis = v * (0.5 - (theta * theta / 48.0));
    else
        axis =  v * (sinp / theta);

    return Quaternion(cosp, axis);
}

Vector3D log(const Quaternion &q)
{
    Vector3D axis = q.getAxis();
    double angle = 2.0 * acos(q[0]);

    return axis * angle;
}

bool reparameterise(Vector3D &v)
{
    double theta  = sqrt(v * v);
    bool status = false;

    if (theta > cutoffAngle)
    {
        double scale = theta;
        if (theta > twoPi)
        {
            theta = fmod(theta, twoPi);
            scale = theta / scale;
            v = v * scale;
            status = true;
        }
        if (theta > cutoffAngle)
        {
            scale = theta;
            theta = twoPi - theta;
            scale = 1.0 - (twoPi / scale);
            v  = v * scale;
            status = true;
        }
    }

    return status;
}

// NRMatrix<double> dRPdQ(const Vector3D &point, const Quaternion &rotation)
// {
//     NRMatrix<double> jacobian(3, 4);
//     double n1 = rotation[0], x1 = rotation[1], y1 = rotation[2], z1 = rotation[3];
//     double x2 = point[0], y2 = point[1], z2 = point[2];
//     double n1x2 = n1 * x2, n1y2 = n1 * y2, n1z2 = n1 * z2;
//     double x1x2 = x1 * x2, x1y2 = x1 * y2, x1z2 = x1 * z2;
//     double y1x2 = y1 * x2, y1y2 = y1 * y2, y1z2 = y1 * z2;
//     double z1x2 = z1 * x2, z1y2 = z1 * y2, z1z2 = z1 * z2;
//
//     jacobian(0, 0) = 2.0 * (n1x2 + y1z2 - z1y2);
//     jacobian(1, 0) = 2.0 * (n1y2 + z1x2 - x1z2);
//     jacobian(2, 0) = 2.0 * (n1z2 + x1y2 - y1x2);
//
//     jacobian(0, 1) = 2.0 * (x1x2 + y1y2 + z1z2);
//     jacobian(1, 1) = 2.0 * (y1x2 - n1z2 - x1y2);
//     jacobian(2, 1) = 2.0 * (n1y2 + z1x2 - x1z2);
//
//     jacobian(0, 2) = 2.0 * (n1z2 + x1y2 - y1x2);
//     jacobian(1, 2) = 2.0 * (x1x2 + y1y2 + z1z2);
//     jacobian(2, 2) = 2.0 * (z1y2 - n1x2 - y1z2);
//
//     jacobian(0, 3) = 2.0 * (x1z2 - n1y2 - z1x2);
//     jacobian(1, 3) = 2.0 * (n1x2 + y1z2 - z1y2);
//     jacobian(2, 3) = 2.0 * (x1x2 + y1y2 + z1z2);
//
//     return jacobian;
// }
//
// NRMatrix<double> dQdE(const Vector3D &v)
// {
//     double theta = sqrt(v * v);
//     double cosp = cos(0.5 * theta);
//     double sinp = sin(0.5 * theta);
//
//     NRMatrix<double> jacobian(4, 3);
//     for (unsigned int i = 0; i < 3; i++)
//     {
//         if (theta < minAngle)
//         {
//             double Tsinc = 0.5 - (theta * theta / 48.0);
//             double vTerm = v[i] * ((theta * theta / 40.0) - 1.0) / 24.0;
//
//             jacobian(0, i) = -0.5 * v[i] * Tsinc;
//             jacobian(1, i) = v[0] * vTerm;
//             jacobian(2, i) = v[1] * vTerm;
//             jacobian(3, i) = v[2] * vTerm;
//
//             jacobian(i + 1, i) += Tsinc;
//         }
//         else
//         {
//             double ang = 1.0 / theta;
//             double sang = sinp * ang;
//             double ang2 = ang * ang * v[i];
//             double cterm = ang2 * (0.5 * cosp - sang);
//
//             jacobian(0, i) = -0.5 * v[i] * sang;
//             jacobian(1, i) = cterm * v[0];
//             jacobian(2, i) = cterm * v[1];
//             jacobian(3, i) = cterm * v[2];
//
//             jacobian(i + 1, i) += sang;
//         }
//     }
//
//     return jacobian;
// }
