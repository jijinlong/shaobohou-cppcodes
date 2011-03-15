#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <sstream>

extern const double EPSILON;

class Vector3D
{
    public:
        Vector3D();
        Vector3D(double x, double y, double z);
        ~Vector3D();

        double magnitude() const;
        Vector3D direction() const;

        void normalise();

        Vector3D& operator+=(const Vector3D &u);
        Vector3D& operator-=(const Vector3D &u);
        Vector3D& operator*=(double s);
        Vector3D& operator/=(double s);

        Vector3D operator-() const;
        Vector3D operator+(const Vector3D &u) const;
        Vector3D operator-(const Vector3D &u) const;
        Vector3D operator/(double s) const;

        friend Vector3D operator*(const Vector3D &u, double s);
        friend Vector3D operator*(double s, const Vector3D &u);

        Vector3D operator^(const Vector3D &u) const;    //cross product
        double operator*(const Vector3D &u) const;      //dot product

        //given three vector, return the normal of the plane they all in, assume CCW winding
        static Vector3D normal(const Vector3D &v1, const Vector3D &v2, const Vector3D &v3);
        static double angle(const Vector3D &v1, const Vector3D &v2);	//angle between 2 directional vector
        static double distance(const Vector3D &v1, const Vector3D &v2);  //euclidean distance between the 2 vector point
        static Vector3D getFloatVersion(const Vector3D &doubleVersion);   //odd function to cast the members to float accuracy but still store as double

        //a convenient accessor and mutator, 0 = X, 1 = Y, 2 = Z
        double& operator[](unsigned int selection);
        const double& operator[](unsigned int selection) const;

        std::string toString() const;

    private:
        double x, y, z;
};

inline std::ostream& operator<<(std::ostream &out, const Vector3D &v)
{
    out << v.toString();
    return out;
}

#endif
