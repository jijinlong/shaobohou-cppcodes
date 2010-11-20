#include "Cylinder.h"

Cylinder::Cylinder()
: m_length(0.0)
{}

Cylinder::Cylinder(const Ellipse2D &top, const Ellipse2D &bot, double length)
: m_top(top), m_bot(bot), m_length(length)
{}

Cylinder::~Cylinder()
{}

const Ellipse2D& Cylinder::top() const
{
    return m_top;
}

const Ellipse2D& Cylinder::bot() const
{
    return m_bot;
}

const double& Cylinder::length() const
{
    return m_length;
}

bool Cylinder::computeEdges(const Vector3D &test_point, Vector3D &top_right, Vector3D &bot_right, Vector3D &top_left,  Vector3D &bot_left) const
{
    Vector2D test_point2d(test_point[0], test_point[1]);

    Vector2D top_right2d, top_left2d;
    int top_status = top().computeExtremalPoints(test_point2d, top_right2d, top_left2d);
    top_right[0] = top_right2d[0]; top_right[1] = top_right2d[1]; top_right[2] = m_length;
    top_left[0]  = top_left2d[0];  top_left[1]  = top_left2d[1];  top_left[2] = m_length;

    Vector2D bot_right2d, bot_left2d;
    int bot_status = bot().computeExtremalPoints(test_point2d, bot_right2d, bot_left2d);
    bot_right[0] = bot_right2d[0]; bot_right[1] = bot_right2d[1]; bot_right[2] = 0.0;
    bot_left[0]  = bot_left2d[0];  bot_left[1]  = bot_left2d[1];  bot_left[2] = 0.0;

    return (top_status != 0) || (bot_status != 0);
}

bool Cylinder::computeNearestPoints(const Vector3D &test_point, Vector3D &top_point, Vector3D &bot_point) const
{
    Vector2D test_point2d(test_point[0], test_point[1]);

    Vector2D top2d, bot2d;
    bool top_status = top().computeNearestPoint(test_point2d, top2d);
    bool bot_status = bot().computeNearestPoint(test_point2d, bot2d);

    return top_status && bot_status;
}
