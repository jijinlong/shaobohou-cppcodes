#ifndef CYLINDER_H
#define CYLINDER_H

#include "Ellipse2D.h"
#include "Vector3D.h"

class Cylinder
{
    public:
        Cylinder();
        Cylinder(const Ellipse2D &top, const Ellipse2D &bot, double length);
        virtual ~Cylinder();

        const Ellipse2D& top() const;
        const Ellipse2D& bot() const;
        const double& length() const;

        // assume test_point is in the Cylinder's coordinate system, line1 on the right/+x and line2 on left/-x
        bool computeEdges(const Vector3D &test_point, Vector3D &top_right, Vector3D &bot_right,
                                                      Vector3D &top_left,  Vector3D &bot_left) const;

        bool computeNearestPoints(const Vector3D &test_point, Vector3D &top_point, Vector3D &bot_point) const;

    private:
        Ellipse2D m_top, m_bot;
        double m_length;
};

#endif
