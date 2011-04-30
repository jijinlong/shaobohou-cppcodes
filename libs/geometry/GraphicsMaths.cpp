#include "GraphicsMaths.h"

#include "Constants.h"

#include <cmath>
#include <cassert>
#include <algorithm>

using std::vector;


const int LEFT_OF_LINE = 1;
const int ON_THE_LINE = 0;
const int RIGHT_OF_LINE = -1;

unsigned int solveQuadratic(double a, double b, double c, double &result0, double &result1)
{
    double determinant = b * b - 4.0 * a * c;

    if (determinant > EPSILON)	//has real roots
    {
        double d_2 = sqrt(determinant);
        double t0 = (-b - d_2) / (2.0 * a);
        double t1 = (-b + d_2) / (2.0 * a);

        if (t0 > t1)
        {
            result0 = t0;
            result1 = t1;
        }
        else
        {
            result0 = t1;
            result1 = t0;
        }

        return 2;
    }
    else if (fabs(determinant) < EPSILON)
    {
        result0 = (- b - sqrt(determinant)) / (2.0 * a);
        return 1;
    }
    else
        return 0;
}

int pointLineRelation(const Vector2D &start, const Vector2D &end, const Vector2D &point)
{
    double orientation = (end[0] - start[0]) * (point[1] - start[1]) - (point[0] - start[0]) * (end[1] - start[1]);

    if (orientation > EPSILON)
        return LEFT_OF_LINE;
    else if (orientation < -EPSILON)
        return RIGHT_OF_LINE;
    else
        return ON_THE_LINE;
}

int pointLineRelation(const LineSegment<Vector2D> &line, const Vector2D &point)
{
    return pointLineRelation(line.start, line.end, point);
}

int pointLineRelationXY(const Vector3D &start, const Vector3D &end, const Vector3D &point)
{
    return pointLineRelation(Vector2D(start[0], start[1]), Vector2D(end[0], end[1]), Vector2D(point[0], point[1]));
}


Vector3D getClosestPointOnLine(const Vector3D &line_direction, const Vector3D &line_origin, const Vector3D &point, double &t)
{
    Vector3D lineOriginToPoint = point - line_origin;
    t = lineOriginToPoint * line_direction;

    return line_origin + (line_direction * t);
}


bool intersectLineLine(const Vector2D &line1_start, const Vector2D &line1_end, const Vector2D &line2_start, const Vector2D &line2_end, double &u1, double &u2)
{
    const Vector2D &p1 = line1_start;
    const Vector2D &p2 = line1_end;
    const Vector2D &p3 = line2_start;
    const Vector2D &p4 = line2_end;

    double den =  (p4[1]-p3[1])*(p2[0]-p1[0]) - (p4[0]-p3[0])*(p2[1]-p1[1]);
    if(fabs(den) < 1e-6)
        return false;

    double num1 = (p4[0]-p3[0])*(p1[1]-p3[1]) - (p4[1]-p3[1])*(p1[0]-p3[0]);
    double num2 = (p2[0]-p1[0])*(p1[1]-p3[1]) - (p2[1]-p1[1])*(p1[0]-p3[0]);

    u1 = num1/den;
    u2 = num2/den;

    return (u1 > 0.0) && (u1 < 1.0) && (u2 > 0.0) && (u2 < 1.0);
}

bool intersectLineLine(const LineSegment<Vector2D> &line1, const LineSegment<Vector2D> &line2, double &u1, double &u2)
{
    return intersectLineLine(line1.start, line1.end, line2.start, line2.end, u1, u2);
}

bool intersectLineLineXY(const Vector3D &line1_start, const Vector3D &line1_end, const Vector3D &line2_start, const Vector3D &line2_end, double &u1, double &u2)
{
    return intersectLineLine(Vector2D(line1_start[0], line1_start[1]), Vector2D(line1_end[0], line1_end[1]),
                             Vector2D(line2_start[0], line2_start[1]), Vector2D(line2_end[0], line2_end[1]),
                             u1, u2);
}

bool intersectLinePlane(const LineSegment<Vector3D> &line, const Plane3D &plane, double &u)
{
    const Vector3D &p1 = line.start;
    const Vector3D &p2 = line.end;
    const Vector3D &p3 = plane.point;
    const Vector3D &n = plane.normal;

    double den = n*(p2-p1);
    if(fabs(den) < 1e-6)
        return false;

    double num = n*(p3-p1);
    u = num/den;

    return true;
}



vector<LineSegment<Vector2D> > clipLinesByPolygon(const vector<LineSegment<Vector2D> > &lines, const ConvexPolygon<Vector2D> &polygon)
{
    vector<LineSegment<Vector2D> > clipped_lines;
    for(unsigned int i = 0; i < lines.size(); i++)
    {
        // if line is completely inside of polygon, no intersections are possible
        bool line_start_in_poly = isPointInsideConvexPolygon(polygon, lines[i].start);
        bool line_end_in_poly = isPointInsideConvexPolygon(polygon, lines[i].end);
        if(line_start_in_poly && line_end_in_poly)
            continue;

        // find intersection coeffs
        vector<double> coeffs;
        for(unsigned int j = 0; j < polygon.size(); j++)
        {
            double u1 = 0.0, u2 = 0.0;
            if(intersectLineLine(lines[i], polygon.lines(j), u1, u2))
                coeffs.push_back(u1);
        }

        // sort and compute intersection points
        sort(coeffs.begin(), coeffs.end());
        vector<Vector2D> intersections;
        for(unsigned int j = 0; j < coeffs.size(); j++)
            intersections.push_back(lines[i].computePoint(coeffs[j]));
        coeffs.clear();

        // created clipped lines
        if(intersections.size() > 0)
        {
            unsigned int start_ind = 0;
            if(!line_start_in_poly)
            {
                clipped_lines.push_back(LineSegment<Vector2D>(lines[i].start, intersections[0]));
                start_ind = 1;
            }

            if(intersections.size() > start_ind)
            {
                // deal with concave polygon case
                for(unsigned int k = start_ind; k < intersections.size()-1; k+=2)
                    clipped_lines.push_back(LineSegment<Vector2D>(intersections[i], intersections[i+1]));

                // deal with exit
                if(!line_end_in_poly)
                    clipped_lines.push_back(LineSegment<Vector2D>(intersections.back(), lines[i].end));
            }
        }
        else
            clipped_lines.push_back(lines[i]);
    }

    return clipped_lines;
}

bool isPointInsideConvexPolygon(const ConvexPolygon<Vector2D> &polygon, const Vector2D &point)
{
    assert(polygon.size() > 2);

    int init_status = pointLineRelation(polygon.lines(0), point);
    for(unsigned int i = 1; i < polygon.size(); i++)
    {
        int new_status = pointLineRelation(polygon.lines(i), point);
        if(new_status != init_status)
            return false;
    }

    // true if the point is on the same side of all line of the polygon
    return true;
}

bool intersectRaySphere(const Ray &ray, const Sphere &sphere, double &t)
{
    double A = ray.direction * ray.direction;
    double B = 2.0 * (ray.direction * (ray.origin - sphere.position));
    double C = (ray.origin - sphere.position) * (ray.origin - sphere.position) - sphere.radius * sphere.radius;

    double t0, t1;
    unsigned numRoots = solveQuadratic(A, B, C, t0, t1);

    if(numRoots == 2)
    {
        if(t1 >= 0.0)
            t = t1;
        else if(t0 >= 0.0)
            t = t0;
        else
            return false;
    }
    else if(numRoots == 1)
    {
        if(t0 >= 0.0)
            t = t0;
        else
            return false;
    }
    else if(numRoots == 0)
        return false;
    else
        assert(false);

    return true;;
}



void polygonClipping(const vector<Vector3D> &clippee, const vector<Vector3D> &clipper, vector<Vector3D> &polygon)
{
    assert((clippee.size() > 0) && (clipper.size() > 2));

    polygon = clippee;

    for (unsigned int i = 0; i < clipper.size(); i++)
    {
        if (polygon.size() == 0)
            return;

        vector<Vector3D>::iterator it = polygon.begin();
        Vector3D lineStart = clipper[i];
        Vector3D lineEnd = clipper[(i + 1) % clipper.size()];
        Vector3D first =  polygon[0];
        Vector3D prev = polygon[0];

        //check first point
        if (pointLineRelationXY(lineStart, lineEnd, polygon[0]) == RIGHT_OF_LINE)
            it = polygon.erase(it);
        else
            it++;

        while (it != polygon.end())
        {
            Vector3D current = (*it);
            int prevStatus = pointLineRelationXY(lineStart, lineEnd, prev);
            int currentStatus = pointLineRelationXY(lineStart, lineEnd, current);

            if (((prevStatus == LEFT_OF_LINE) && (currentStatus == RIGHT_OF_LINE)) ||
                    ((prevStatus == RIGHT_OF_LINE) && (currentStatus == LEFT_OF_LINE)))
            { //intersecting clipping line, need to compute intersection
                Vector3D intersection;
//                 lineIntersection(intersection, lineStart, lineEnd, prev, current);
                double u1 = 0.0, u2 = 0.0;
                intersectLineLineXY(lineStart, lineEnd, prev, current, u1, u2);
                intersection = lineStart+(lineEnd-lineStart)*u1;
                intersection[2] = 0.0;
                it = polygon.insert(it, intersection);
                it++; //move back to current as insert() insert in front of it

                if (currentStatus == RIGHT_OF_LINE)
                    it = polygon.erase(it);    //it moved to next element of vector
                else
                    it++;
            }
            else if (currentStatus == RIGHT_OF_LINE) //current point not in clipper and no intersection
                it = polygon.erase(it);
            else
                it++;

            prev = current;
        }

        //closing the polygon
        if (polygon.size() > 2)
        {
            int prevStatus = pointLineRelationXY(lineStart, lineEnd, prev);
            int firstStatus = pointLineRelationXY(lineStart, lineEnd, first);
            if (((prevStatus == LEFT_OF_LINE) && (firstStatus == RIGHT_OF_LINE)) ||
                    ((prevStatus == RIGHT_OF_LINE) && (firstStatus == LEFT_OF_LINE)))
            {
                Vector3D intersection;
//                 lineIntersection(intersection, lineStart, lineEnd, prev, first);
                double u1 = 0.0, u2 = 0.0;
                intersectLineLineXY(lineStart, lineEnd, prev, first, u1, u2);
                intersection = lineStart+(lineEnd-lineStart)*u1;
                intersection[2] = 0.0;
                it = polygon.insert(it, intersection);
            }
        }

    }
}



bool computePlane(const vector<Vector3D> &points, const Vector3D &normal, Vector3D &origin, Vector3D &xAxis, Vector3D &yAxis)
{
    if (points.size() < 2)
        return false;

    origin = points[0];

    Vector3D diff;
    for (unsigned int i = 1; i < points.size(); i++) //find an abitrary xAxis
    {
        diff =  points[i] - points[0];
        if ((diff * diff) > EPSILON)
        {
            xAxis = diff;
            xAxis.normalise();
            break;
        }
    }

    if ((xAxis * xAxis) < EPSILON)  //all points are too close together
        return false;

    Quaternion toY = Quaternion::makeFromAxisAngle(normal, PI*0.5);
    yAxis = toY.rotate(xAxis);

    return true;
}

bool planeMap(const vector<Vector3D> &points, const Vector3D &origin, const Vector3D &xAxis, const Vector3D &yAxis, vector<Vector3D> &newPoints)
{
    if (((xAxis * xAxis) < EPSILON) || ((yAxis * yAxis) < EPSILON))
        return false;

    double x, y;

    for (unsigned int i = 0; i < points.size(); i++)
    {
        getClosestPointOnLine(xAxis, origin, points[i], x);
        getClosestPointOnLine(yAxis, origin, points[i], y);
        newPoints.push_back(Vector3D(x, y, 0));
    }

    return true;
}

bool inversePlaneMap(const vector<Vector3D> &points, const Vector3D &origin, const Vector3D &xAxis, const Vector3D &yAxis, vector<Vector3D> &newPoints)
{
    if (((xAxis * xAxis) < EPSILON) || ((yAxis * yAxis) < EPSILON))
        return false;

    for (unsigned int i = 0; i < points.size(); i++)
        newPoints.push_back(origin + xAxis * points[i][0] + yAxis * points[i][1]);

    return true;
}
