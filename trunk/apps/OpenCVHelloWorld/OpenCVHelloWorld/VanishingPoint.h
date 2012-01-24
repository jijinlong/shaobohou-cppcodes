#ifndef VANISHING_POINT_H
#define VANISHING_POINT_H


#include "Geometry.h"

#include <fstream>
#include <iostream>


// vanishing point
class VanishingPoint : public Selectable
{
public:
    VanishingPoint() : m_point(new Point2D()), m_atInfinity(false) {}
    VanishingPoint(const LineSegment &line) : m_point(new Point2D()), m_atInfinity(false)
    {
        addLine(line);
    }

    ~VanishingPoint()
    {
        delete m_point;
        for(unsigned int i = 0; i < lines.size(); i++)
        {
            delete lines[i];
        }
    }

    // stub function
    int select(int x, int y)
    {
        return std::numeric_limits<int>::max();
    }

    // stub function
    void update(int x, int y)
    {
        assert(lines.size() > 1);

        // update vanishing point
        m_atInfinity = intersectInfiniteLines(*lines[0], *lines[1], *m_point);
    }

    // register function
    void registerCascade(SelectableGroup &selectables)
    {
        for(unsigned int i = 0; i < lines.size(); i++)
        {
            selectables.registerObject(this, lines[i]);
            lines[i]->registerCascade(selectables);
        }
    }

    // a new line constraint to the vanishing set
    void addLine(const LineSegment &line)
    {
        lines.push_back(new LineSegment(line));
    }

    const Point2D& point() const
    {
        return *m_point;
    }

    bool atInfinity() const
    {
        return m_atInfinity;
    }

    void render(IplImage *temp, const CvScalar &col) const
    {
        for(unsigned int i = 0; i < lines.size(); i++)
        {
            lines[i]->render(temp, col, 2);
        }

        if(lines.size() > 1)
        {
            if(!m_atInfinity)
            {
                cvLine(temp, cvPoint(*m_point), cvPoint(*m_point), col, 10);
            }
        }
    }

    void save(std::ofstream &out) const
    {
        out << lines.size() << std::endl;
        for(unsigned int i = 0; i < lines.size(); i++)
        {
            out << lines[i]->beg().x() << " " << lines[i]->beg().y() << "  ";
            out << lines[i]->end().x() << " " << lines[i]->end().y() << std::endl;;
        }
    }

    void load(std::ifstream &in)
    {
        int nlines = 0;
        in >> nlines;

        lines.clear();
        for(int i = 0; i < nlines; i++)
        {
            int bx, by, ex, ey;
            in >> bx >> by;
            in >> ex >> ey;
            addLine(LineSegment(Point2D(bx, by), Point2D(ex, ey)));
        }

        update(0, 0);
    }

private:
    Point2D *m_point;
    bool m_atInfinity;
    std::vector<LineSegment*> lines;
};


#endif
