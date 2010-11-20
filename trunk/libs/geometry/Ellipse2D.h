#ifndef ELLIPSE2D_H
#define ELLIPSE2D_H

#include "Vector2D.h"

// assume the ellipse is centered on the origin and its principal axes are aligned with x and y axis
class Ellipse2D
{
    public:
        Ellipse2D();
        Ellipse2D(double xradius, double yradius);
        virtual ~Ellipse2D();

        const double& xradius() const;
        const double& yradius() const;

        // assume test_point is in the ellipse's coordinate system, point1 on the right/+x and point2 on left/-x
        int computeExtremalPoints(const Vector2D &test_point, Vector2D &point1, Vector2D &point2) const;
        bool computeNearestPoint(const Vector2D &test_point, Vector2D &nearest_point) const;

    private:
        double m_xradius, m_yradius;
        double a, b, c;
};

#endif
