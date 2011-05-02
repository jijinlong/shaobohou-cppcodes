#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <sstream>
#include <cassert>

class Vector2D
{
    public:
        Vector2D();
        Vector2D(double x, double y);
        virtual ~Vector2D();

        double lengthSquared() const;

        Vector2D operator+(const Vector2D &u) const;
        Vector2D operator-(const Vector2D &u) const;

        //a convenient accessor and mutator, 0 = X, 1 = Y
        double& operator[](unsigned int selection);
        const double& operator[](unsigned int selection) const;

        std::string toString() const;

    private:
        double x, y;
};

Vector2D operator*(const Vector2D &v, double u);
Vector2D operator/(const Vector2D &v, double u);
double operator*(const Vector2D &v, const Vector2D &u);

inline std::ostream& operator<<(std::ostream &out, const Vector2D &v)
{
    out << v.toString();
    return out;
}

#endif
