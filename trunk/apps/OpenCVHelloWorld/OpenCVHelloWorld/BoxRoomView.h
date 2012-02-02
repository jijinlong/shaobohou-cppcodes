#ifndef BOX_ROOM_VIEW_H
#define BOX_ROOM_VIEW_H


#include "VanishingPoint.h"


// Wall Boundary
class BoxRoomView : public Selectable
{
public:
    BoxRoomView() : m_handles(new Point2D[4]), m_vpoint1(0), m_vpoint2(0), m_vpoint3(0) {}

    ~BoxRoomView()
    {
        delete [] m_handles;
    }

    // setup function
    void setup(const Point2D &first, const Point2D &second, const std::vector<VanishingPoint*> &vanishings, const int width, const int height)
    {
        // compute the bounding box
        const Point2D::Real Xmin = std::min(first.x(), second.x());
        const Point2D::Real Xmax = std::max(first.x(), second.x());
        const Point2D::Real Ymin = std::min(first.y(), second.y());
        const Point2D::Real Ymax = std::max(first.y(), second.y());

        // initialise control points in clockwise winding
        m_handles[0].set(Point2D((Xmin+Xmax)/2, Ymin));
        m_handles[1].set(Point2D(Xmax, (Ymin+Ymax)/2));
        m_handles[2].set(Point2D((Xmin+Xmax)/2, Ymax));
        m_handles[3].set(Point2D(Xmin, (Ymin+Ymax)/2));

        // call the real setup function
        setup(vanishings, width, height);
    }

    // setup function
    void setup(const std::vector<VanishingPoint*> &vanishings, const int width, const int height)
    {
        // compute distances of vanishing points to the centre of the image
        const Point2D::Real dist0 =  Point2D(vanishings[0]->point()).dist2(Point2D(width/2, height/2));
        const Point2D::Real dist1 =  Point2D(vanishings[1]->point()).dist2(Point2D(width/2, height/2));
        const Point2D::Real dist2 =  Point2D(vanishings[2]->point()).dist2(Point2D(width/2, height/2));

        // choose the closest vanishing point to construct the back wall
        if(dist0 <= std::min(dist1, dist2))
        {
            m_vpoint1 = vanishings[1];
            m_vpoint2 = vanishings[2];
            m_vpoint3 = vanishings[0];
        }
        else if(dist1 <= std::min(dist0, dist2))
        {
            m_vpoint1 = vanishings[0];
            m_vpoint2 = vanishings[2];
            m_vpoint3 = vanishings[1];
        }
        else // if(dist2 <= std::min(dist0, dist1))
        {
            m_vpoint1 = vanishings[0];
            m_vpoint2 = vanishings[1];
            m_vpoint3 = vanishings[2];
        }

        // swap vanishing points if necessary, the correct order 
        // should form a quadrilaterial with longer edges
        Point2D corners[4];
        computeCorners(corners, *m_vpoint1, *m_vpoint2);
        const Point2D::Real len1 = corners[0].dist2(corners[1]);
        computeCorners(corners, *m_vpoint2, *m_vpoint1);
        const Point2D::Real len2 = corners[0].dist2(corners[1]);
        if(len2 > len1)
        {
            std::swap(m_vpoint1, m_vpoint2);
        }

        // recompute the corners and control points
        computeCorners(corners, *m_vpoint1, *m_vpoint2);
        m_handles[0].set((corners[3]+corners[0])/2);
        m_handles[1].set((corners[0]+corners[1])/2);
        m_handles[2].set((corners[1]+corners[2])/2);
        m_handles[3].set((corners[2]+corners[3])/2);
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
        selectables.registerObject(this, &m_handles[0]);
        selectables.registerObject(this, &m_handles[1]);
        selectables.registerObject(this, &m_handles[2]);
        selectables.registerObject(this, &m_handles[3]);
    }

    // recompute the corners of the back wall
    void computeCorners(Point2D corners[4], const VanishingPoint &vpoint1, const VanishingPoint &vpoint2) const
    {
        corners[0] = Point2D(intersectInfiniteLines(LineSegment(Point2D(vpoint1.point()), m_handles[0]), LineSegment(Point2D(vpoint2.point()), m_handles[1])));
        corners[1] = Point2D(intersectInfiniteLines(LineSegment(Point2D(vpoint1.point()), m_handles[2]), LineSegment(Point2D(vpoint2.point()), m_handles[1])));
        corners[2] = Point2D(intersectInfiniteLines(LineSegment(Point2D(vpoint1.point()), m_handles[2]), LineSegment(Point2D(vpoint2.point()), m_handles[3])));
        corners[3] = Point2D(intersectInfiniteLines(LineSegment(Point2D(vpoint1.point()), m_handles[0]), LineSegment(Point2D(vpoint2.point()), m_handles[3])));
    }

    // render function
    void render(IplImage *temp, const CvScalar &col, const int thickness) const
    {
        // compute corners of the wall
        if(m_vpoint1 && m_vpoint2 && m_vpoint3)
        {
            Point2D corners[4];
            computeCorners(corners, *m_vpoint1, *m_vpoint2);
            //intersectInfiniteLines(LineSegment(m_vpoint1->point(), m_handles[0]), LineSegment(m_vpoint2->point(), m_handles[1]), corners[0]);
            //intersectInfiniteLines(LineSegment(m_vpoint1->point(), m_handles[2]), LineSegment(m_vpoint2->point(), m_handles[1]), corners[1]);
            //intersectInfiniteLines(LineSegment(m_vpoint1->point(), m_handles[2]), LineSegment(m_vpoint2->point(), m_handles[3]), corners[2]);
            //intersectInfiniteLines(LineSegment(m_vpoint1->point(), m_handles[0]), LineSegment(m_vpoint2->point(), m_handles[3]), corners[3]);

            // draw wall edges
            cvLine(temp, cvPoint(corners[0]), cvPoint(corners[1]), col, thickness);
            cvLine(temp, cvPoint(corners[1]), cvPoint(corners[2]), col, thickness);
            cvLine(temp, cvPoint(corners[2]), cvPoint(corners[3]), col, thickness);
            cvLine(temp, cvPoint(corners[3]), cvPoint(corners[0]), col, thickness);

            // draw the edges between walls and floors/ceilings
            cvLine(temp, cvPoint(corners[0]+(corners[0]-Point2D(m_vpoint3->point()))*10), cvPoint(corners[0]), col, thickness);
            cvLine(temp, cvPoint(corners[1]+(corners[1]-Point2D(m_vpoint3->point()))*10), cvPoint(corners[1]), col, thickness);
            cvLine(temp, cvPoint(corners[2]+(corners[2]-Point2D(m_vpoint3->point()))*10), cvPoint(corners[2]), col, thickness);
            cvLine(temp, cvPoint(corners[3]+(corners[3]-Point2D(m_vpoint3->point()))*10), cvPoint(corners[3]), col, thickness);

            // draw control points
            cvLine(temp, cvPoint(m_handles[0]), cvPoint(m_handles[0]), CV_RGB(0, 0, 0), thickness*5);
            cvLine(temp, cvPoint(m_handles[1]), cvPoint(m_handles[1]), CV_RGB(0, 0, 0), thickness*5);
            cvLine(temp, cvPoint(m_handles[2]), cvPoint(m_handles[2]), CV_RGB(0, 0, 0), thickness*5);
            cvLine(temp, cvPoint(m_handles[3]), cvPoint(m_handles[3]), CV_RGB(0, 0, 0), thickness*5);
        }
    }

    // output function
    void save(std::ofstream &out) const
    {
        out << 4 << std::endl;
        out << m_handles[0] << std::endl;
        out << m_handles[1] << std::endl;
        out << m_handles[2] << std::endl;
        out << m_handles[3] << std::endl;

        // save wall corners
        Point2D corners[4];
        computeCorners(corners, *m_vpoint1, *m_vpoint2);
        //intersectInfiniteLines(LineSegment(m_vpoint1->point(), m_handles[0]), LineSegment(m_vpoint2->point(), m_handles[1]), corners[0]);
        //intersectInfiniteLines(LineSegment(m_vpoint1->point(), m_handles[2]), LineSegment(m_vpoint2->point(), m_handles[1]), corners[1]);
        //intersectInfiniteLines(LineSegment(m_vpoint1->point(), m_handles[2]), LineSegment(m_vpoint2->point(), m_handles[3]), corners[2]);
        //intersectInfiniteLines(LineSegment(m_vpoint1->point(), m_handles[0]), LineSegment(m_vpoint2->point(), m_handles[3]), corners[3]);

        out << 4 << std::endl;
        out << corners[0] << std::endl;
        out << corners[1] << std::endl;
        out << corners[2] << std::endl;
        out << corners[3] << std::endl;
    }

    // input function
    bool load(std::ifstream &in)
    {
        int nhandles = 0;
        in >> nhandles;

        if(nhandles==4)
        {
            for(int i = 0; i < nhandles; i++)
            {
                Point2D temp;
                in >> temp;
                m_handles[i].set(temp);
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
