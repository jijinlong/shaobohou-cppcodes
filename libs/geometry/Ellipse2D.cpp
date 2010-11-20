#include "Ellipse2D.h"

#include <cmath>

using std::swap;

Ellipse2D::Ellipse2D()
: m_xradius(0.0), m_yradius(0.0), a(0.0), b(0.0), c(0.0)
{}

Ellipse2D::Ellipse2D(double xradius, double yradius)
: m_xradius(xradius), m_yradius(yradius), a(1.0/(xradius*xradius)), b(1.0/(yradius*yradius)), c(1.0)
{}

Ellipse2D::~Ellipse2D()
{}

const double& Ellipse2D::xradius() const
{
    return m_xradius;
}

const double& Ellipse2D::yradius() const
{
    return m_yradius;
}

int Ellipse2D::computeExtremalPoints(const Vector2D &test_point, Vector2D &point1, Vector2D &point2) const
{
    double x0 = test_point[0];
    double y0 = test_point[1];

    double m = a*x0;
    double n = b*y0;

    if(fabs(m) > 0.000001)
    {
        double mm = c/m;
        double nn = -n/m;

        double aa = a*nn*nn + b;
        double bb = 2.0*a*mm*nn;
        double cc = -c + a*mm*mm;

        double det = bb*bb - 4.0*aa*cc;
        if(det < 0.0) return 1;     // test_point is inside the ellipse

        double sqrt_det = sqrt(det);
        point1[1] = (-bb + sqrt_det) / (2.0*aa);
        point2[1] = (-bb - sqrt_det) / (2.0*aa);

        point1[0] = mm + nn*point1[1];
        point2[0] = mm + nn*point2[1];
    }
    else if(fabs(n) > 0.000001)
    {
        double mm = c/n;
        double nn = -m/n;

        double aa = a + b*nn*nn;
        double bb = 2.0*b*mm*nn;
        double cc = -c + b*mm*mm;

        double det = bb*bb - 4.0*aa*cc;
        if(det < 0.0) return 2;     // test_point is inside the ellipse

        double sqrt_det = sqrt(det);
        point1[0] = (-bb + sqrt_det) / (2.0*aa);
        point2[0] = (-bb - sqrt_det) / (2.0*aa);

        point1[1] = mm + nn*point1[0];
        point2[1] = mm + nn*point2[0];
    }
    else
        return 3;

    if(((test_point[0]-0.0)*(point1[1]-0.0) - (point1[0]-0.0)*(test_point[1]-0.0)) < 0.0)
        swap(point1, point2);

    return 0;
}

bool Ellipse2D::computeNearestPoint(const Vector2D &test_point, Vector2D &nearest_point) const
{
    if((test_point*test_point) < 1e-8)
        return false;

#if 0
    double angle = atan2(test_point[1], test_point[0]);
    nearest_point[0] = xradius()*cos(angle);
    nearest_point[1] = yradius()*sin(angle);
#else
    double len = sqrt(test_point*test_point);
    nearest_point[0] = xradius()*test_point[0]/len;
    nearest_point[1] = yradius()*test_point[1]/len;
#endif

    return true;
}
