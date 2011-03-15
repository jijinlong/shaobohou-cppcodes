#ifndef QUATERNION_H
#define QUATERNION_H

#include "Vector3D.h"

class Quaternion
{
    public:
        Quaternion();
        Quaternion(double w, const Vector3D &v);
        Quaternion(double w, double x, double y, double z);
        ~Quaternion();

        double magnitude() const;
        Vector3D getVector() const;
        double getScalar() const;
        void normalise();

        Quaternion& operator+=(const Quaternion &q);
        Quaternion& operator-=(const Quaternion &q);
        Quaternion& operator*=(double s);
        Quaternion& operator/=(double s);
        Quaternion operator~() const;          //negate the axis of rotation, reverse the rotation

        Quaternion operator+(const Quaternion &q) const;
        Quaternion operator-(const Quaternion &q) const;
        Quaternion operator*(const Quaternion &q) const;  //quaternion multiplication, rotation stacking, read from right to left
        Quaternion operator/(double s) const;

        friend Quaternion operator*(const Quaternion &q, double s);
        friend Quaternion operator*(double s, const Quaternion &q);
        friend Quaternion operator*(const Quaternion &q, const Vector3D &v);  //q * Quaternnion(0.0, v)
        friend Quaternion operator*(const Vector3D &v, const Quaternion &q);  //Quaternnion(0.0, v) * q

        double getAngle() const;
        Vector3D getAxis() const;
        Quaternion rotate(const Quaternion &q) const;	//rotate q by this
        Vector3D rotate(const Vector3D &v) const;		//rotate q by this

        static Quaternion makeFromAxisAngle(const Vector3D &axis, double angle); 	//take angle in radian
        static Quaternion makeFromEulerAngles(double roll, double pitch, double yaw); 	//takes parameters in radian
        Vector3D makeEulerAngles() const;			//return answer in radians

        double& operator[](unsigned int selection);
        const double& operator[](unsigned int selection) const;

        //implemented but untested slerp interpolation function
        static Quaternion slerp(const Quaternion &from, const Quaternion &to, double t);

        std::string toString() const;

    private:
        double w, x, y, z;
};

inline std::ostream& operator<<(std::ostream &out, const Quaternion &q)
{
    out << q.toString();
    return out;
}

double computeAlignment(const Quaternion &source_quat, const Quaternion &target_quat, const Vector3D &rotation_axis);

#endif
