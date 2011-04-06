#ifndef MATRIX3X3_H
#define MATRIX3X3_H

#include "Vector3D.h"
#include "Quaternion.h"

class Matrix3x3
{
    public:
        Matrix3x3();
        Matrix3x3(double e00, double e01, double e02,
                  double e10, double e11, double e12,
                  double e20, double e21, double e22);
        Matrix3x3(const Vector3D &col, const Vector3D &row);
        Matrix3x3(const Quaternion &quaternion);
        ~Matrix3x3();

        double determinant() const;
        Matrix3x3 transpose() const;
        Matrix3x3 inverse() const;
        static Matrix3x3 identity();

        Matrix3x3& operator+=(const Matrix3x3 &m);
        Matrix3x3& operator-=(const Matrix3x3 &m);
        Matrix3x3& operator*=(double s);
        Matrix3x3& operator/=(double s);

        Matrix3x3 operator+(const Matrix3x3 &m) const;
        Matrix3x3 operator-(const Matrix3x3 &m) const;
        Matrix3x3 operator/(double s) const;
        friend Matrix3x3 operator*(const Matrix3x3 &m, double s);
        friend Matrix3x3 operator*(double s, const Matrix3x3 &m);

        Matrix3x3 operator*(const Matrix3x3 &m) const;
        Matrix3x3& operator*=(const Matrix3x3 &m);
        friend Vector3D operator*(const Matrix3x3 &m, const Vector3D &u);
        friend Vector3D operator*(const Vector3D &u, const Matrix3x3 &m);

        Quaternion makeQuaternion() const;

        //eigenvalues[i] match normalised eigenvector[i]
        void computeEigenSystem(Vector3D &eigenvalues, Vector3D &eigenvector0, Vector3D &eigenvector1, Vector3D &eigenvector2) const;

        //accessor and mutator
        double& operator()(unsigned int row, unsigned int col);
        const double& operator()(unsigned int row, unsigned int col) const;

        std::string toString() const;

    private:
        double e[3][3];   //e[row][column]
};

inline std::ostream& operator<<(std::ostream &out, const Matrix3x3 &m)
{
    out << m.toString();
    return out;
}

#endif
