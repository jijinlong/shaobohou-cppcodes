#ifndef VANISHING_POINT_H
#define VANISHING_POINT_H


#include "Geometry.h"

#include <fstream>
#include <iostream>


using namespace Geometry;


// vanishing point
class VanishingPoint : public Selectable
{
public:
    VanishingPoint() {}
    VanishingPoint(const LineSegment &line)
    {
        addLine(line);
    }

    ~VanishingPoint()
    {
        for(unsigned int i = 0; i < m_lines.size(); i++)
        {
            delete m_lines[i];
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
        assert(m_lines.size() > 1);

        // update vanishing point
        m_point = intersectInfiniteLines(*m_lines[0], *m_lines[1]);
    }

    // register function
    void registerCascade(SelectableGroup &selectables)
    {
        for(unsigned int i = 0; i < m_lines.size(); i++)
        {
            selectables.registerObject(this, m_lines[i]);
            m_lines[i]->registerCascade(selectables);
        }
    }

    // a new line constraint to the vanishing set
    void addLine(const LineSegment &line)
    {
        m_lines.push_back(new LineSegment(line));
    }

    // accessor for the vanishing point
    const HomgPoint2D& point() const
    {
        return m_point;
    }

    // rendering function
    void render(IplImage *temp, const CvScalar &col) const
    {
        // render vanishing lines
        for(unsigned int i = 0; i < m_lines.size(); i++)
        {
            m_lines[i]->render(temp, col, 2);
        }

        // render vanishing points
        if(m_lines.size() > 1)
        {
            cvLine(temp, cvPoint(Point2D(m_point)), cvPoint(Point2D(m_point)), col, 10);
        }
    }

    // output function
    void save(std::ofstream &out) const
    {
        out << m_lines.size() << std::endl;
        for(unsigned int i = 0; i < m_lines.size(); i++)
        {
            out << *m_lines[i] << std::endl;;
        }
    }

    // input function
    void load(std::ifstream &in)
    {
        int nlines = 0;
        in >> nlines;

        m_lines.clear();
        for(int i = 0; i < nlines; i++)
        {
            LineSegment temp;
            in >> temp;
            addLine(temp);
        }

        // compute auxiliary parameters
        update(0, 0);
    }

private:
    HomgPoint2D m_point;
    std::vector<LineSegment*> m_lines;
};


#endif
