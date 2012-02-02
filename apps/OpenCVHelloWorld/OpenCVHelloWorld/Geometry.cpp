#include "Geometry.h"

namespace Geometry
{;

// construct from a cartesian point
HomgPoint2D::HomgPoint2D(const Point2D &other) : m_vec(other.x(), other.y(), 1)
{}

// construct a homogeneous 2d line by joining two homgeneous 2d points
HomgPoint2D::HomgPoint2D(const HomgLine2D &line1, const HomgLine2D &line2)
    : m_vec(line1.m_vec.cross(line2.m_vec))
{}


// construct a homogeneous 2d point by intersecting two homgeneous 2d lines
HomgLine2D::HomgLine2D(const HomgPoint2D &point1, const HomgPoint2D &point2)
    : m_vec(point1.m_vec.cross(point2.m_vec))
{}


// construct a cartesian point from a homgeneous point
Point2D::Point2D(const HomgPoint2D &p) : m_x(p.x()/p.w()), m_y(p.y()/p.w())
{}


// I/O operators
std::ostream& operator<<(std::ostream &out, const Point2D &point)
{
    out << static_cast<int>(point.m_x+0.5) << " " << static_cast<int>(point.m_y+0.5);

    return out;
}

std::istream& operator>>(std::istream &in, Point2D &point)
{
    in >> point.m_x >> point.m_y;

    return in;
}


// Point2D to CvPoint operator
CvPoint cvPoint(const Point2D &other)
{
    return ::cvPoint(static_cast<int>(other.m_x), static_cast<int>(other.m_y));
}


// I/O operators
std::ostream& operator<<(std::ostream &out, const LineSegment &line)
{
    out << *line.m_beg << "  " << *line.m_end;

    return out;
}

std::istream& operator>>(std::istream &in, LineSegment &line)
{
    in >> *line.m_beg >> *line.m_end;

    return in;
}


// computes the intersection of two infinite line in homogeneous 2D coordinate
HomgPoint2D intersectInfiniteLines(const LineSegment &line1, const LineSegment &line2)
{
    HomgLine2D hline1(HomgPoint2D(line1.beg()), HomgPoint2D(line1.end()));
    HomgLine2D hline2(HomgPoint2D(line2.beg()), HomgPoint2D(line2.end()));
    return HomgPoint2D(hline1, hline2);
}


}
