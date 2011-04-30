#include "Matrix3x3.h"

#include "Constants.h"
#include "special.h"

#include <cassert>
#include <cmath>
#include <string>

using std::endl;
using std::string;
using std::ostringstream;


Matrix3x3::Matrix3x3()
{
    e[0][0] = 0.0;   e[0][1] = 0.0;    e[0][2] = 0.0;
    e[1][0] = 0.0;   e[1][1] = 0.0;    e[1][2] = 0.0;
    e[2][0] = 0.0;   e[2][1] = 0.0;    e[2][2] = 0.0;
}

Matrix3x3::Matrix3x3(   double e00, double e01, double e02,
                        double e10, double e11, double e12,
                        double e20, double e21, double e22)
{
    e[0][0] = e00;   e[0][1] = e01;    e[0][2] = e02;
    e[1][0] = e10;   e[1][1] = e11;    e[1][2] = e12;
    e[2][0] = e20;   e[2][1] = e21;    e[2][2] = e22;
}

Matrix3x3::Matrix3x3(const Vector3D &col, const Vector3D &row)
{
    e[0][0] = col[0]*row[0];   e[0][1] = col[0]*row[1];    e[0][2] = col[0]*row[2];
    e[1][0] = col[1]*row[0];   e[1][1] = col[1]*row[1];    e[1][2] = col[1]*row[2];
    e[2][0] = col[2]*row[0];   e[2][1] = col[2]*row[1];    e[2][2] = col[2]*row[2];
}

Matrix3x3::Matrix3x3(const Quaternion &quaternion)
{
    const double w = quaternion[0], x = quaternion[1], y = quaternion[2], z = quaternion[3];
    double q00 = w*w, q11 = x*x, q22 = y*y, q33 = z*z;

    e[0][0] = q00 + q11 - q22 - q33;    e[0][1] = 2.0 * (x*y - w*z);        e[0][2] = 2.0 * (x*z + w*y),
    e[1][0] = 2.0 * (x*y + w*z);        e[1][1] = q00 - q11 + q22 - q33;    e[1][2] = 2.0 * (y*z - w*x),
    e[2][0] = 2.0 * (x*z - w*y);        e[2][1] = 2.0 * (y*z + w*x);        e[2][2] = q00 - q11 - q22 + q33;
}

Matrix3x3::~Matrix3x3()
{
}

double Matrix3x3::determinant() const
{
    return e[0][0]*e[1][1]*e[2][2] - e[0][0]*e[2][1]*e[1][2] +
           e[1][0]*e[2][1]*e[0][2] - e[1][0]*e[0][1]*e[2][2] +
           e[2][0]*e[0][1]*e[1][2] - e[2][0]*e[1][1]*e[0][2];
}

Matrix3x3 Matrix3x3::transpose() const
{
    return Matrix3x3(  e[0][0], e[1][0], e[2][0],
                       e[0][1], e[1][1], e[2][1],
                       e[0][2], e[1][2], e[2][2]);
}

Matrix3x3 Matrix3x3::inverse() const
{
    double d = determinant();

    if (fabs(d) < EPSILON) d = 1.0;

    Matrix3x3 m(e[1][1]*e[2][2] - e[1][2]*e[2][1], e[0][2]*e[2][1] - e[0][1]*e[2][2], e[0][1]*e[1][2] - e[0][2]*e[1][1],
                e[1][2]*e[2][0] - e[1][0]*e[2][2], e[0][0]*e[2][2] - e[0][2]*e[2][0], e[0][2]*e[1][0] - e[0][0]*e[1][2],
                e[1][0]*e[2][1] - e[1][1]*e[2][0], e[0][1]*e[2][0] - e[0][0]*e[2][1], e[0][0]*e[1][1] - e[0][1]*e[1][0]);

    m /= d;

    return m;
}

Matrix3x3 Matrix3x3::identity()
{
    return Matrix3x3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
}

Matrix3x3& Matrix3x3::operator+=(const Matrix3x3 &m)
{
    e[0][0] += m.e[0][0];  e[0][1] += m.e[0][1];   e[0][2] += m.e[0][2];
    e[1][0] += m.e[1][0];  e[1][1] += m.e[1][1];   e[1][2] += m.e[1][2];
    e[2][0] += m.e[2][0];  e[2][1] += m.e[2][1];   e[2][2] += m.e[2][2];

    return *this;
}

Matrix3x3& Matrix3x3::operator-=(const Matrix3x3 &m)
{
    e[0][0] -= m.e[0][0];  e[0][1] -= m.e[0][1];   e[0][2] -= m.e[0][2];
    e[1][0] -= m.e[1][0];  e[1][1] -= m.e[1][1];   e[1][2] -= m.e[1][2];
    e[2][0] -= m.e[2][0];  e[2][1] -= m.e[2][1];   e[2][2] -= m.e[2][2];

    return *this;
}

Matrix3x3& Matrix3x3::operator*=(double s)
{
    e[0][0] *= s;  e[0][1] *= s;   e[0][2] *= s;
    e[1][0] *= s;  e[1][1] *= s;   e[1][2] *= s;
    e[2][0] *= s;  e[2][1] *= s;   e[2][2] *= s;

    return *this;
}

Matrix3x3& Matrix3x3::operator/=(double s)
{
    e[0][0] /= s;  e[0][1] /= s;   e[0][2] /= s;
    e[1][0] /= s;  e[1][1] /= s;   e[1][2] /= s;
    e[2][0] /= s;  e[2][1] /= s;   e[2][2] /= s;

    return *this;
}

Matrix3x3 Matrix3x3::operator+(const Matrix3x3 &m) const
{
    return Matrix3x3(e[0][0] + m.e[0][0], e[0][1] + m.e[0][1], e[0][2] + m.e[0][2],
                     e[1][0] + m.e[1][0], e[1][1] + m.e[1][1], e[1][2] + m.e[1][2],
                     e[2][0] + m.e[2][0], e[2][1] + m.e[2][1], e[2][2] + m.e[2][2]);
}

Matrix3x3 Matrix3x3::operator-(const Matrix3x3 &m) const
{
    return Matrix3x3(e[0][0] - m.e[0][0], e[0][1] - m.e[0][1], e[0][2] - m.e[0][2],
                     e[1][0] - m.e[1][0], e[1][1] - m.e[1][1], e[1][2] - m.e[1][2],
                     e[2][0] - m.e[2][0], e[2][1] - m.e[2][1], e[2][2] - m.e[2][2]);
}

Matrix3x3 Matrix3x3::operator/(double s) const
{
    return Matrix3x3(e[0][0] / s, e[0][1] / s, e[0][2] / s,
                     e[1][0] / s, e[1][1] / s, e[1][2] / s,
                     e[2][0] / s, e[2][1] / s, e[2][2] / s);
}

Matrix3x3 operator*(const Matrix3x3 &m, double s)
{
    return Matrix3x3(m.e[0][0] * s, m.e[0][1] * s, m.e[0][2] * s,
                     m.e[1][0] * s, m.e[1][1] * s, m.e[1][2] * s,
                     m.e[2][0] * s, m.e[2][1] * s, m.e[2][2] * s);
}

Matrix3x3 operator*(double s, const Matrix3x3 &m)
{
    return Matrix3x3(m.e[0][0] * s, m.e[0][1] * s, m.e[0][2] * s,
                     m.e[1][0] * s, m.e[1][1] * s, m.e[1][2] * s,
                     m.e[2][0] * s, m.e[2][1] * s, m.e[2][2] * s);
}

Matrix3x3 Matrix3x3::operator*(const Matrix3x3 &m) const
{
    return Matrix3x3(e[0][0]*m.e[0][0] + e[0][1]*m.e[1][0] + e[0][2]*m.e[2][0],
                     e[0][0]*m.e[0][1] + e[0][1]*m.e[1][1] + e[0][2]*m.e[2][1],
                     e[0][0]*m.e[0][2] + e[0][1]*m.e[1][2] + e[0][2]*m.e[2][2],
                     e[1][0]*m.e[0][0] + e[1][1]*m.e[1][0] + e[1][2]*m.e[2][0],
                     e[1][0]*m.e[0][1] + e[1][1]*m.e[1][1] + e[1][2]*m.e[2][1],
                     e[1][0]*m.e[0][2] + e[1][1]*m.e[1][2] + e[1][2]*m.e[2][2],
                     e[2][0]*m.e[0][0] + e[2][1]*m.e[1][0] + e[2][2]*m.e[2][0],
                     e[2][0]*m.e[0][1] + e[2][1]*m.e[1][1] + e[2][2]*m.e[2][1],
                     e[2][0]*m.e[0][2] + e[2][1]*m.e[1][2] + e[2][2]*m.e[2][2]);
}

Matrix3x3& Matrix3x3::operator*=(const Matrix3x3 &m)
{
    Matrix3x3 t = *this;

    e[0][0] = t.e[0][0]*m.e[0][0] + t.e[0][1]*m.e[1][0] + t.e[0][2]*m.e[2][0];
    e[0][1] = t.e[0][0]*m.e[0][1] + t.e[0][1]*m.e[1][1] + t.e[0][2]*m.e[2][1];
    e[0][2] = t.e[0][0]*m.e[0][2] + t.e[0][1]*m.e[1][2] + t.e[0][2]*m.e[2][2];
    e[1][0] = t.e[1][0]*m.e[0][0] + t.e[1][1]*m.e[1][0] + t.e[1][2]*m.e[2][0];
    e[1][1] = t.e[1][0]*m.e[0][1] + t.e[1][1]*m.e[1][1] + t.e[1][2]*m.e[2][1];
    e[1][2] = t.e[1][0]*m.e[0][2] + t.e[1][1]*m.e[1][2] + t.e[1][2]*m.e[2][2];
    e[2][0] = t.e[2][0]*m.e[0][0] + t.e[2][1]*m.e[1][0] + t.e[2][2]*m.e[2][0];
    e[2][1] = t.e[2][0]*m.e[0][1] + t.e[2][1]*m.e[1][1] + t.e[2][2]*m.e[2][1];
    e[2][2] = t.e[2][0]*m.e[0][2] + t.e[2][1]*m.e[1][2] + t.e[2][2]*m.e[2][2];

    return *this;
}

Vector3D operator*(const Matrix3x3 &m, const Vector3D &u)
{
    double x = u[0], y = u[1], z = u[2];

    return Vector3D(m.e[0][0]*x + m.e[0][1]*y + m.e[0][2]*z,
                    m.e[1][0]*x + m.e[1][1]*y + m.e[1][2]*z,
                    m.e[2][0]*x + m.e[2][1]*y + m.e[2][2]*z);
}

Vector3D operator*(const Vector3D &u, const Matrix3x3 &m)
{
    double x = u[0], y = u[1], z = u[2];

    return Vector3D(x*m.e[0][0] + y*m.e[1][0] + z*m.e[2][0],
                    x*m.e[0][1] + y*m.e[1][1] + z*m.e[2][1],
                    x*m.e[0][2] + y*m.e[1][2] + z*m.e[2][2]);
}

Quaternion Matrix3x3::makeQuaternion() const
{
    const Matrix3x3 &matrix = *this;
    const double m00 = matrix(0, 0), m11 = matrix(1, 1), m22 = matrix(2, 2);
    const double tr = m00 + m11 + m22 + 1.0;

    double w, x, y, z;
    if (tr > EPSILON)
    {
        double s = 0.5 / sqrt(tr);
        w = 0.25 / s;

        x = (matrix(2, 1) - matrix(1, 2)) * s;
        y = (matrix(0, 2) - matrix(2, 0)) * s;
        z = (matrix(1, 0) - matrix(0, 1)) * s;
    }
    else
    {
        unsigned int i = 0;
        if (m11 > m00) i = 1;
        if (m22 > matrix(i, i)) i = 2;

        unsigned int j = (i + 1) % 3;
        unsigned int k = (j + 1) % 3;

        double s = sqrt(matrix(i, i) - matrix(j, j) + matrix(k, k) + 1.0);

        double q[4];
        q[i] = s * 0.5;
        if (fabs(s) > EPSILON) s = 0.5 / s;

        q[3] = (matrix(j, k) - matrix(k, j)) * s;
        q[j] = (matrix(i, j) + matrix(j, i)) * s;
        q[k] = (matrix(i, k) + matrix(k, i)) * s;

        w = q[3]; x = q[0]; y = q[1]; z = q[2];
    }

    Quaternion q(w, Vector3D(x, y, z));
    q.normalise();

    return q;
}

void Matrix3x3::computeEigenSystem(Vector3D &eigenvalues, Vector3D &eigenvector0, Vector3D &eigenvector1, Vector3D &eigenvector2) const
{
    double e0220 = e[0][2] * e[2][0], e0110 = e[0][1] * e[1][0], e1221 = e[1][2] * e[2][1];
    double e0011 = e[0][0] * e[1][1], e0022 = e[0][0] * e[2][2], e1122 = e[1][1] * e[2][2];

    //c3 always = -1
    double c2 = e[0][0] + e[1][1] + e[2][2];
    double c1 = e0220 + e0110 + e1221 - e0011 - e0022 - e1122;
    double c0 = e0011 * e[2][2] + e[0][1] * e[1][2] * e[2][0] + e[0][2] * e[1][0] * e[2][1] - e0220 * e[1][1] - e0110 * e[2][2] - e[0][0] * e1221;
    double r[3];
    int maxDistinctRealRootsNumber = solve_cubic(-1.0, c2, c1, c0, r[0], r[1], r[2]);

    //multiplicity of 3, A = I * r[i]
    if ((maxDistinctRealRootsNumber == 1) || ((maxDistinctRealRootsNumber == 2) && (fabs(r[0] - r[1]) < EPSILON)))
    {
        eigenvalues = Vector3D(r[0], 0.0, 0.0);
        eigenvector0 = Vector3D(1.0, 0.0, 0.0);
        eigenvector1 = Vector3D(0.0, 1.0, 0.0);
        eigenvector2 = Vector3D(0.0, 0.0, 1.0);
        return;
    }

    if (maxDistinctRealRootsNumber == 3)
        eigenvalues = Vector3D(r[0], r[1], r[2]);
    else if (maxDistinctRealRootsNumber == 2)
        eigenvalues = Vector3D(r[0], r[1], 0.0);

    for (int i = 0; i < maxDistinctRealRootsNumber; i++)
    {
        int multiplicity = 1;
        if (maxDistinctRealRootsNumber == 2) //given how solve cubic works, if an eigenvalue and the next eigenvalue are the same then multiplicity of 2
        {
            if (fabs(r[i] - r[(i + 1) % 3]) < EPSILON)
                multiplicity = 2;
        }

        //m = (A - I * r[i])
        Matrix3x3 m = *this - (Matrix3x3::identity() * eigenvalues[i]);

        //u[0] = row 1 of m ^ row 2 of m
        //u[1] = row 2 of m ^ row 0 of m
        //u[2] = row 0 of m ^ row 1 of m
        Vector3D u[3];
        u[0] = Vector3D(m.e[1][0], m.e[1][1], m.e[1][2]) ^ Vector3D(m.e[2][0], m.e[2][1], m.e[2][2]);
        u[1] = Vector3D(m.e[2][0], m.e[2][1], m.e[2][2]) ^ Vector3D(m.e[0][0], m.e[0][1], m.e[0][2]);
        u[2] = Vector3D(m.e[0][0], m.e[0][1], m.e[0][2]) ^ Vector3D(m.e[1][0], m.e[1][1], m.e[1][2]);

        int row = -1;
        int column = -1;
        double max = -1.0;

        if (multiplicity == 1)
        {
            for (int j = 0; j < 3; j++)
                for (int k = 0; k < 3; k++)
                    if ((column < 0) || (fabs(u[j][k]) > max))
                    {
                        column = j;
                        max = fabs(u[j][k]);
                    }

            u[column].normalise();
            if (i == 0) eigenvector0 = u[column];
            else if (i == 1) eigenvector1 = u[column];
            else if (i == 2) eigenvector2 = u[column];
        }
        else if (multiplicity == 2)
        {
            row = -1;
            column = -1;
            max = -1.0;
            for (int j = 0; j < 3; j++)
                for (int k = 0; k < 3; k++)
                    if ((column < 0) || (fabs(m(j, k)) > max))
                    {
                        row = j;
                        column = k;
                        max = fabs(m(j, k));
                    }

            Vector3D tempEigenvector[2];

            if (row == 0)
            {
                if ((column == 0) || (column == 1))
                {
                    tempEigenvector[0] = Vector3D(-m(0, 1), m(0, 0), 0.0);
                    tempEigenvector[1] = Vector3D(-m(0, 2) * m(0, 0), -m(0, 2) * m(0, 1), m(0, 0) * m(0, 0) + m(0, 1) * m(0, 1));
                }
                else if (column == 2)
                {
                    tempEigenvector[0] = Vector3D(m(0, 2), 0.0, -m(0, 0));
                    tempEigenvector[1] = Vector3D(-m(0, 1) * m(0, 0), m(0, 0) * m(0, 0) + m(0, 2) * m(0, 2), -m(0, 1) * m(0, 2));
                }
                else
                    assert(false);
            }
            else if (row == 1)
            {
                if ((column == 1) || (column = 2))
                {
                    tempEigenvector[0] = Vector3D(0.0, -m(1, 2), m(1, 1));
                    tempEigenvector[1] = Vector3D(m(1, 1) * m(1, 1) + m(1, 2) * m(1, 2), -(m(0, 1)) * m(1, 1), -m(0, 1) * m(1, 2));
                }
                else
                    assert(false);
            }
            else if (row == 2)
            {
                if (column == 2)
                {
                    tempEigenvector[0] = Vector3D(0.0, -m(2, 2), m(1, 1));
                    tempEigenvector[1] = Vector3D(m(1, 2) * m(1, 2) + m(2, 2) * m(2, 2), -m(0, 2) * m(1, 2), -m(0, 2) * m(2, 2));
                }
                else
                    assert(false);
            }

            if (i == 0)
            {
                eigenvector0 = tempEigenvector[0];
                eigenvector1 = tempEigenvector[1];
            }
            else if (i == 1)
            {
                eigenvector1 = tempEigenvector[0];
                eigenvector2 = tempEigenvector[1];
            }

            i++;
            eigenvector0.normalise();
            eigenvector1.normalise();
            eigenvector2.normalise();
        }
        else
            assert(false);
    }
}

const double& Matrix3x3::operator()(unsigned int row, unsigned int col) const
{
    assert((row < 3) && (col < 3));
    return e[row][col];
}

double& Matrix3x3::operator()(unsigned int row, unsigned int col)
{
    assert((row < 3) && (col < 3));
    return e[row][col];
}

string Matrix3x3::toString() const
{
    ostringstream s;
    s << e[0][0] << " " << e[0][1] << " " << e[0][2] << endl
      << e[1][0] << " " << e[1][1] << " " << e[1][2] << endl
      << e[2][0] << " " << e[2][1] << " " << e[2][2];

    return s.str();
}
