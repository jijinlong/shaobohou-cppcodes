#ifndef GEOMETRY_H
#define GEOMETRY_H


#include  "Selectable.h"

#include <cv.h>


namespace Geometry 
{;

// floating point type
typedef double Real;


// pre-declaration
class Vector2D;
class Vector3D;
class HomgPoint2D;
class HomgLine2D;
class Point2D;
class LineSegment;


// 2D Vector
class Vector2D
{
public:
    Vector2D() : m_x(0), m_y(0) {};
    Vector2D(const Real &nx, const Real &ny) : m_x(nx), m_y(ny) {};

    Real& x() { return m_x; }
    Real& y() { return m_y; }

    const Real& x() const { return m_x; }
    const Real& y() const { return m_y; }

private:
    Real m_x, m_y;
};


// 3D Vector
class Vector3D
{
public:
    Vector3D() : m_x(0), m_y(0), m_z(0) {};
    Vector3D(const Real &nx, const Real &ny, const Real &nz) : m_x(nx), m_y(ny), m_z(nz) {};

    Real& x() { return m_x; }
    Real& y() { return m_y; }
    Real& z() { return m_z; }

    const Real& x() const { return m_x; }
    const Real& y() const { return m_y; }
    const Real& z() const { return m_z; }

    Vector3D cross(const Vector3D &other) const
    {
        const Real nx = this->m_y*other.m_z - this->m_z*other.m_y;
        const Real ny = this->m_z*other.m_x - this->m_x*other.m_z;
        const Real nz = this->m_x*other.m_y - this->m_y*other.m_x;

        return Vector3D(nx, ny, nz);
    }

private:
    Real m_x, m_y, m_z;
};


// 2D Point in homogeneous coordinate
class HomgPoint2D
{
public:
    friend class HomgLine2D;

    HomgPoint2D() :  m_vec(0, 0, 1) {}
    HomgPoint2D(const Real &nx, const Real &ny, const Real &nw=1) :  m_vec(nx, ny, nw) {}
    explicit HomgPoint2D(const Vector3D &other) : m_vec(other) {}

    // construct from a cartesian point
    explicit HomgPoint2D(const Point2D &other);

    // construct a homogeneous 2d line by joining two homgeneous 2d points
    HomgPoint2D(const HomgLine2D &line1, const HomgLine2D &line2);

    Real& x() { return m_vec.x(); }
    Real& y() { return m_vec.y(); }
    Real& w() { return m_vec.z(); }

    const Real& x() const { return m_vec.x(); }
    const Real& y() const { return m_vec.y(); }
    const Real& w() const { return m_vec.z(); }

    bool atInfinity(const Real tol=0) const
    {
        return fabs(w()) <= tol;
    }

private:
    Vector3D m_vec;
};


// 2D Line in homogeneous coordinate
class HomgLine2D
{
public:
    friend class HomgPoint2D;

    HomgLine2D() :  m_vec(0, 0, 1) {}
    HomgLine2D(const Real &na, const Real &nb, const Real &nc) :  m_vec(na, nb, nc) {}
    explicit HomgLine2D(const Vector3D &other) : m_vec(other) {}

    // construct from cartesian line
    explicit HomgLine2D(const LineSegment &line);

    // construct a homogeneous 2d point by intersecting two homgeneous 2d lines
    HomgLine2D(const HomgPoint2D &point1, const HomgPoint2D &point2);

    Real& a() { return m_vec.x(); }
    Real& b() { return m_vec.y(); }
    Real& c() { return m_vec.z(); }

    const Real& a() const { return m_vec.x(); }
    const Real& b() const { return m_vec.y(); }
    const Real& c() const { return m_vec.z(); }

private:
    Vector3D m_vec;
};


// 2D point
class Point2D : public Selectable
{
public:
    friend CvPoint cvPoint(const Point2D &other);

    Point2D() : m_x(0), m_y(0) {}
    Point2D(const Real x, const Real y) : m_x(x), m_y(y) {}
    Point2D(const Point2D &other) : m_x(other.m_x), m_y(other.m_y) {}

    // construct a cartesian point from a homgeneous point
    explicit Point2D(const HomgPoint2D &p);

    Point2D& operator=(const Point2D &other)
    {
        set(other);
        return *this;
    }

    // select function
    int select(int x, int y)
    {
        return static_cast<int>(dist2(Point2D(x, y)));
    }

    // update function
    void update(int x, int y)
    {
        set(x, y);
    }

    // stub function
    void registerCascade(SelectableGroup &selectables)
    {
    }


    // accessors and modifiers
    const Real& x() const
    {
        return m_x;
    }

    const Real& y() const
    {
        return m_y;
    }

    void set(const Real x, const Real y)
    {
        m_x = x;
        m_y = y;
    }

    void set(const Point2D &other)
    {
        m_x = other.m_x;
        m_y = other.m_y;
    }


    // test equality at integer coordinates
    bool equals(const Point2D &other) const
    {
        return static_cast<int>(this->m_x)==static_cast<int>(other.m_x) && 
            static_cast<int>(this->m_y)==static_cast<int>(other.m_y);
    }

    // squared distance function
    Real dist2(const Point2D &other) const
    {
        const Real dx = this->m_x - other.m_x;
        const Real dy = this->m_y - other.m_y;

        return dx*dx + dy*dy;
    }


    // arithmetic operators
    Point2D operator-(const Point2D &other) const
    {
        return Point2D(m_x-other.m_x, m_y-other.m_y);
    }

    Point2D operator+(const Point2D &other) const
    {
        return Point2D(m_x+other.m_x, m_y+other.m_y);
    }

    Point2D operator*(const Real s) const
    {
        return Point2D(m_x*s, m_y*s);
    }

    Point2D operator/(const Real s) const
    {
        return Point2D(m_x/s, m_y/s);
    }

    Real dot(const Point2D &other) const
    {
        return m_x*other.x() + m_y*other.y();
    }

    Real length2() const
    {
        return this->dot(*this);
    }


    friend std::ostream& operator<<(std::ostream &out, const Point2D &point);
    friend std::istream& operator>>(std::istream &in, Point2D &point);


private:
    Real m_x, m_y;
};

// I/O operators
std::ostream& operator<<(std::ostream &out, const Point2D &point);
std::istream& operator>>(std::istream &in, Point2D &point);

// Point2D to CvPoint operator
CvPoint cvPoint(const Point2D &other);


// Line Class and Functions
class LineSegment : public Selectable
{
public:
    LineSegment() : m_beg(new Point2D()), m_end(new Point2D()) {}
    LineSegment(const Point2D &beg) : m_beg(new Point2D(beg)), m_end(0) {}
    LineSegment(const Point2D &beg, const Point2D &end) : m_beg(new Point2D(beg)), m_end(new Point2D(end)) {}
    LineSegment(const LineSegment &other) : m_beg(new Point2D(*other.m_beg)), m_end(new Point2D(*other.m_end)) {}

    ~LineSegment()
    {
        if(m_beg) delete m_beg;
        if(m_end) delete m_end;
    }

    // stub function
    int select(int x, int y)
    {
        return std::numeric_limits<int>::max();
    }

    // stub function
    void update(int x, int y)
    {
    }

    // register function
    void registerCascade(SelectableGroup &selectables)
    {
        selectables.registerObject(this, m_beg);
        m_beg->registerCascade(selectables);

        selectables.registerObject(this, m_end);
        m_end->registerCascade(selectables);
    }


    // accessors and modifiers
    const Point2D& beg() const
    {
        return *m_beg;
    }

    const Point2D& end() const
    {
        return *m_end;
    }

    void setBeg(const Point2D &beg)
    {
        m_beg->set(beg);
    }

    void setEnd(const Point2D &end)
    {
        m_end->set(end);
    }


    // distance function
    Real dist2line(const Point2D &point)
    {
        const Real u = (point-*m_beg).dot(*m_end-*m_beg) / (*m_end-*m_beg).length2();
        const Point2D closestPointOnLine(*m_beg + (*m_end-*m_beg)*u);

        return closestPointOnLine.dist2(point);
    }


    // rendering function
    void render(IplImage *temp, const CvScalar &col, const int thickness) const
    {
        cvLine(temp, cvPoint(*m_beg), cvPoint(*m_end), col, thickness);
        cvLine(temp, cvPoint(*m_beg), cvPoint(*m_beg), CV_RGB(0, 0, 0), thickness*5);
        cvLine(temp, cvPoint(*m_end), cvPoint(*m_end), CV_RGB(0, 0, 0), thickness*5);
    }

    friend std::ostream& operator<<(std::ostream &out, const LineSegment &line);
    friend std::istream& operator>>(std::istream &in, LineSegment &line);

private:
    Point2D *m_beg;
    Point2D *m_end;
};

// I/O operators
std::ostream& operator<<(std::ostream &out, const LineSegment &line);
std::istream& operator>>(std::istream &in, LineSegment &line);

// computes the intersection of two infinite line in homogeneous 2D coordinate
HomgPoint2D intersectInfiniteLines(const LineSegment &line0, const LineSegment &line1);


}


#endif
