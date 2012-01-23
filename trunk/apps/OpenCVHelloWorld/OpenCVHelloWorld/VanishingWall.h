#ifndef VANISHING_WALL_H
#define VANISHING_WALL_H


#include "VanishingPoint.h"


// Wall Boundary
class VanishingWall : public Selectable
{
public:
    VanishingWall() : m_handles(new Point2D[4]), m_vpoint1(0), m_vpoint2(0), m_vpoint3(0) {}

    void setup(const Point2D &first, const Point2D &second, VanishingPoint *vpoint1, VanishingPoint *vpoint2, VanishingPoint *vpoint3)
    {
        if(first.x()<=second.x() && first.y()<=second.y())
        {
            m_handles[0].set(Point2D((first.x()+second.x())/2, first.y()));
            m_handles[1].set(Point2D(second.x(), (first.y()+second.y())/2));
            m_handles[2].set(Point2D((first.x()+second.x())/2, second.y()));
            m_handles[3].set(Point2D(first.x(), (first.y()+second.y())/2));
        }
        else
        {
            m_handles[0].set(Point2D((first.x()+second.x())/2, second.y()));
            m_handles[1].set(Point2D(first.x(), (first.y()+second.y())/2));
            m_handles[2].set(Point2D((first.x()+second.x())/2, first.y()));
            m_handles[3].set(Point2D(second.x(), (first.y()+second.y())/2));
        }

        if(vpoint1->point()->x()<=vpoint2->point()->x() &&vpoint1->point()->y()<=vpoint2->point()->y())
        {
            m_vpoint1 = vpoint1;
            m_vpoint2 = vpoint2;
        }
        else
        {
            m_vpoint1 = vpoint2;
            m_vpoint2 = vpoint1;
        }
        m_vpoint3 = vpoint3;
    }

    void setup(const std::vector<VanishingPoint*> &vanishings, const int width, const int height)
    {
        const Point2D::Real dist0 =  vanishings[0]->point()->dist2(Point2D(width/2, height/2));
        const Point2D::Real dist1 =  vanishings[1]->point()->dist2(Point2D(width/2, height/2));
        const Point2D::Real dist2 =  vanishings[2]->point()->dist2(Point2D(width/2, height/2));

        Point2D::Real Xmin = m_handles[0].x(), Xmax =  m_handles[0].x();
        Point2D::Real Ymin = m_handles[0].y(), Ymax =  m_handles[0].y();
        for(int i = 0; i < 4; i++)
        {
            Xmin = std::min(Xmin, m_handles[i].x());
            Xmax = std::max(Xmax, m_handles[i].x());
            Ymin = std::min(Ymin, m_handles[i].y());
            Ymax = std::max(Ymax, m_handles[i].y());
        }

        if(dist0 <= std::min(dist1, dist2))
        {
            this->setup(Point2D(Xmin, Ymin), Point2D(Xmax, Ymax), vanishings[1], vanishings[2], vanishings[0]);
        }
        else if(dist1 <= std::min(dist0, dist2))
        {
            this->setup(Point2D(Xmin, Ymin), Point2D(Xmax, Ymax), vanishings[0], vanishings[2], vanishings[1]);
        }
        else // if(dist2 <= std::min(dist0, dist1))
        {
            this->setup(Point2D(Xmin, Ymin), Point2D(Xmax, Ymax), vanishings[0], vanishings[1], vanishings[2]);
        }
    }

    // stub function
    int selection(int x, int y)
    {
        return std::numeric_limits<int>::max();
    }

    // stub function
    void update(int x, int y)
    {
    }

    void registerCascade(SelectableGroup &selectables)
    {
        selectables.registerSelectable(this, &m_handles[0]);
        selectables.registerSelectable(this, &m_handles[1]);
        selectables.registerSelectable(this, &m_handles[2]);
        selectables.registerSelectable(this, &m_handles[3]);
    }

    void render(IplImage *temp, const CvScalar &col, const int thickness) const
    {
        // compute corners of the wall
        if(m_vpoint1 && m_vpoint2 && m_vpoint3)
        {
            Point2D corners[4];
            intersectInfiniteLines(LineSegment(*m_vpoint1->point(), m_handles[0]), LineSegment(*m_vpoint2->point(), m_handles[1]), corners[0]);
            intersectInfiniteLines(LineSegment(*m_vpoint1->point(), m_handles[2]), LineSegment(*m_vpoint2->point(), m_handles[1]), corners[1]);
            intersectInfiniteLines(LineSegment(*m_vpoint1->point(), m_handles[2]), LineSegment(*m_vpoint2->point(), m_handles[3]), corners[2]);
            intersectInfiniteLines(LineSegment(*m_vpoint1->point(), m_handles[0]), LineSegment(*m_vpoint2->point(), m_handles[3]), corners[3]);

            // draw wall edges
            cvLine(temp, cvPoint(corners[0]), cvPoint(corners[1]), col, thickness);
            cvLine(temp, cvPoint(corners[1]), cvPoint(corners[2]), col, thickness);
            cvLine(temp, cvPoint(corners[2]), cvPoint(corners[3]), col, thickness);
            cvLine(temp, cvPoint(corners[3]), cvPoint(corners[0]), col, thickness);

            // draw the edges between walls and floors/ceilings
            cvLine(temp, cvPoint(corners[0]+(corners[0]-*m_vpoint3->point())*10), cvPoint(corners[0]), col, thickness);
            cvLine(temp, cvPoint(corners[1]+(corners[1]-*m_vpoint3->point())*10), cvPoint(corners[1]), col, thickness);
            cvLine(temp, cvPoint(corners[2]+(corners[2]-*m_vpoint3->point())*10), cvPoint(corners[2]), col, thickness);
            cvLine(temp, cvPoint(corners[3]+(corners[3]-*m_vpoint3->point())*10), cvPoint(corners[3]), col, thickness);

            // draw control points
            cvLine(temp, cvPoint(m_handles[0]), cvPoint(m_handles[0]), CV_RGB(0, 0, 0), thickness*5);
            cvLine(temp, cvPoint(m_handles[1]), cvPoint(m_handles[1]), CV_RGB(0, 0, 0), thickness*5);
            cvLine(temp, cvPoint(m_handles[2]), cvPoint(m_handles[2]), CV_RGB(0, 0, 0), thickness*5);
            cvLine(temp, cvPoint(m_handles[3]), cvPoint(m_handles[3]), CV_RGB(0, 0, 0), thickness*5);
        }
    }

    void save(std::ofstream &out) const
    {
        out << 4 << std::endl;
        out << m_handles[0].x() << " " << m_handles[0].y() << std::endl;
        out << m_handles[1].x() << " " << m_handles[1].y() << std::endl;
        out << m_handles[2].x() << " " << m_handles[2].y() << std::endl;
        out << m_handles[3].x() << " " << m_handles[3].y() << std::endl;

        // save wall corners
        Point2D corners[4];
        intersectInfiniteLines(LineSegment(*m_vpoint1->point(), m_handles[0]), LineSegment(*m_vpoint2->point(), m_handles[1]), corners[0]);
        intersectInfiniteLines(LineSegment(*m_vpoint1->point(), m_handles[2]), LineSegment(*m_vpoint2->point(), m_handles[1]), corners[1]);
        intersectInfiniteLines(LineSegment(*m_vpoint1->point(), m_handles[2]), LineSegment(*m_vpoint2->point(), m_handles[3]), corners[2]);
        intersectInfiniteLines(LineSegment(*m_vpoint1->point(), m_handles[0]), LineSegment(*m_vpoint2->point(), m_handles[3]), corners[3]);

        out << 4 << std::endl;
        out << corners[0].x() << " " << corners[0].y() << std::endl;
        out << corners[1].x() << " " << corners[1].y() << std::endl;
        out << corners[2].x() << " " << corners[2].y() << std::endl;
        out << corners[3].x() << " " << corners[3].y() << std::endl;
    }

    bool load(std::ifstream &in)
    {
        int nhandles = 0;
        in >> nhandles;

        if(nhandles==4)
        {
            for(int i = 0; i < nhandles; i++)
            {
                Point2D::Real x, y;
                in >> x >> y;
                m_handles[i].set(Point2D(x, y));
            }

            return true;
        }

        return false;
    }

private:
    Point2D *m_handles;
    VanishingPoint *m_vpoint1;
    VanishingPoint *m_vpoint2;
    VanishingPoint *m_vpoint3;
};


#endif
