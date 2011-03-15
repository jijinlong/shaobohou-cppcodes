#ifndef GRAPHICS_MATHS_H
#define GRAPHICS_MATHS_H

#include "Vector2D.h"
#include "Vector3D.h"
#include "Quaternion.h"
#include "Matrix3x3.h"

#include <vector>
#include <cassert>
#include <iostream>


// simple LineSegment class
template <typename T>
class LineSegment
{
    public:
        LineSegment() {}
        LineSegment(const T &line_start, const T &line_end) : start(line_start), end(line_end), diff(end-start) {}

        T computePoint(double u) const
        {
            return start + diff*u;
        }

        LineSegment<T> computeSegment(double u0, double u1) const
        {
            return LineSegment<T>(computePoint(u0), computePoint(u1));
        }

        T start;
        T end;
        T diff;
};


// simple 3D plane class
class Plane3D
{
    public:
        Plane3D() {}
        Plane3D(const Vector3D &p1, const Vector3D &p2, const Vector3D &p3)
        : point(p1), normal(Vector3D::normal(p1, p2, p3)), distance2origin(-(Vector3D()-point)*normal)
        {}

        double distance2plane(const Vector3D &test_point) const
        {
            return (test_point-point)*normal+distance2origin;
        }

        void flip()
        {
            normal = -normal;
            distance2origin = -distance2origin;
        }

        Vector3D point;
        Vector3D normal;
        double distance2origin;
};

// simple ConvexPolygon class
template <typename T>
class ConvexPolygon
{
    public:
        ConvexPolygon() {}
        ConvexPolygon(const std::vector<T> &points) : m_points(points), m_lines(points.size())
        {
            assert(points.size() > 1);
            for(unsigned int i = 0; i < points.size(); i++)
            {
                m_lines[i] = LineSegment<T>(points[(i+0)%points.size()], points[(i+1)%points.size()]);
            }
        }

        unsigned int size() const { return m_points.size(); }

        const T& points(int i) const { return m_points.at(i); }
        const LineSegment<T>& lines(int i) const {return m_lines.at(i); }

        std::vector<T> m_points;
        std::vector<LineSegment<T> > m_lines;
};


// point-line relation
extern const int LEFT_OF_LINE, ON_THE_LINE, RIGHT_FLINE;
int pointLineRelation(const Vector2D &start, const Vector2D &end, const Vector2D &point);
int pointLineRelation(const LineSegment<Vector2D> &line, const Vector2D &point);
int pointLineRelationXY(const Vector3D &start, const Vector3D &end, const Vector3D &point);

// closest point on line
Vector3D getClosestPointOnLine(const Vector3D &line_direction, const Vector3D &line_origin, const Vector3D &point, double &t);

// line-line intersection
bool intersectLineLine(const Vector2D &line1_start, const Vector2D &line1_end, const Vector2D &line2_start, const Vector2D &line2_end, double &u1, double &u2);
bool intersectLineLine(const LineSegment<Vector2D> &line1, const LineSegment<Vector2D> &line2, double &u1, double &u2);
bool intersectLineLineXY(const Vector3D &line1_start, const Vector3D &line1_end, const Vector3D &line2_start, const Vector3D &line2_end, double &u1, double &u2);

// line-plane intersection
bool intersectLinePlane(const LineSegment<Vector3D> &line, const Plane3D &plane, double &u);

// lines-polygon intersection (return lines)
std::vector<LineSegment<Vector2D> > clipLinesByPolygon(const std::vector<LineSegment<Vector2D> > &lines, const ConvexPolygon<Vector2D> &polygon);

// point in convex polygon test
bool isPointInsideConvexPolygon(const ConvexPolygon<Vector2D> &polygon, const Vector2D &point);


// simple Sphere class
class Sphere
{
    public:
        Sphere() : radius(0.0){};
        Sphere(const Vector3D &pos, double r) : position(pos), radius(r) {};

        Vector3D position;
        double radius;
};


//Simple Ray class
class Ray
{
    public:
        Vector3D origin, direction;
};


// ray-sphere intersection
bool intersectRaySphere(const Ray &ray, const Sphere &sphere, double &t);


//polygon clipper clips clippee and stores result int polygon, all CCW winding, clippee have more than 0 vertex, clipper must be at least a triangle, also convex
void polygonClipping(const std::vector<Vector3D> &clippee, const std::vector<Vector3D> &clipper, std::vector<Vector3D> &polygon);


//maps a sets of point that form a plane onto arbitrary 2D plane coordinate, preserve geometric relationship
bool computePlane(const std::vector<Vector3D> &points, const Vector3D &normal, Vector3D &origin, Vector3D &xAxis, Vector3D &yAxis);
bool planeMap(const std::vector<Vector3D> &points, const Vector3D &origin, const Vector3D &xAxis, const Vector3D &yAxis, std::vector<Vector3D> &newPoints);
bool inversePlaneMap(const std::vector<Vector3D> &points, const Vector3D &origin, const Vector3D &xAxis, const Vector3D &yAxis, std::vector<Vector3D> &newPoints);


#endif
