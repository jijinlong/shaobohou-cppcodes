#ifndef GEOMETRY_H
#define GEOMETRY_H


#include  "Selectable.h"

#include <cv.h>


// Point Functions
class Point2D : public Selectable
{
public:
    typedef double Real;

    friend CvPoint cvPoint(const Point2D &other);

    Point2D() : m_x(0), m_y(0) {}
    Point2D(const Real x, const Real y) : m_x(x), m_y(y) {}
    Point2D(const Point2D &other) : m_x(other.m_x), m_y(other.m_y) {}

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
    return cvPoint(static_cast<int>(other.m_x), static_cast<int>(other.m_y));
}


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
    Point2D::Real dist2line(const Point2D &point)
    {
        const Point2D::Real u = (point-*m_beg).dot(*m_end-*m_beg) / (*m_end-*m_beg).length2();
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


// computes the intersection of two infinites as Point2D vpoint
// return true if the intersection point is at infinity
// consider switching to homogenous coordinates?
bool intersectInfiniteLines(const LineSegment &line0, const LineSegment &line1, Point2D &vpoint)
{
    const double x1 = line0.beg().x(); const double y1 = line0.beg().y();
    const double x2 = line0.end().x(); const double y2 = line0.end().y();
    const double x3 = line1.beg().x(); const double y3 = line1.beg().y();
    const double x4 = line1.end().x(); const double y4 = line1.end().y();

    double den =  (y4-y3)*(x2-x1) - (x4-x3)*(y2-y1);
    if(fabs(den) < 1e-6)
    {
        vpoint.set(static_cast<int>(((x2-x1)+(x4-x3))/2), static_cast<int>(((y2-y1)+(y4-y3))/2));

        return true;
    }
    else
    {
        const double num1 = (x4-x3)*(y1-y3) - (y4-y3)*(x1-x3);
        //double num2 = (x2-x1)*(y1-y3) - (y2-y1)*(x1-x3);

        const double u1 = num1/den;
        //u2 = num2/den;

        vpoint.set(x1+u1*(x2-x1), y1+u1*(y2-y1));

        return false;
    }
}


#endif
