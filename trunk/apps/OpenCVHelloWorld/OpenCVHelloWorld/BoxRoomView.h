#ifndef BOX_ROOM_VIEW_H
#define BOX_ROOM_VIEW_H


#include "VanishingPoint.h"


// projective view of a box room defined by three vanishing points
class BoxRoomView : public Selectable
{
public:
    BoxRoomView() : m_vpoint1(0), m_vpoint2(0), m_vpoint3(0), m_handles(new Point2D[4]) {}

    ~BoxRoomView()
    {
        delete [] m_handles;
        if(m_vpoint1) delete m_vpoint1;
        if(m_vpoint2) delete m_vpoint2;
        if(m_vpoint3) delete m_vpoint3;
    }

    // has all three vanishing points been initialised
    int numberOfVanishingPoints() const
    {
        int count = 0;
        if(m_vpoint1) ++count;
        if(m_vpoint2) ++count;
        if(m_vpoint3) ++count;

        return count;
    }

    // has all three vanishing points been initialised
    bool initialisedEnoughVanishingPoints() const
    {
        return numberOfVanishingPoints() >= 3;
    }

    // initialise a new vanishing point if more needed
    VanishingPoint* initialiseNewVanishingPoint(const LineSegment &line)
    {
        // find an uninitialised vanishing point
        VanishingPoint **vpoint = 0;
        if(!m_vpoint1) vpoint = &m_vpoint1;
        if(!m_vpoint2) vpoint = &m_vpoint2;
        if(!m_vpoint3) vpoint = &m_vpoint3;

        // initialise a new vanishing point
        if(vpoint)
        {
            *vpoint = new VanishingPoint(line);
            (*vpoint)->addLine(LineSegment(Point2D(line.beg())+Point2D(10, 10), Point2D(line.end())+Point2D(10, 10)));

            return *vpoint;
        }
        
        return 0;
    }

    // setup function
    void setup(const Point2D &first, const Point2D &second, const Point2D &centre)
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
        setup(centre);
    }

    // setup function
    void setup(const Point2D &centre)
    {
        // compute distances of vanishing points to the centre of the image
        const Point2D::Real dist0 =  Point2D(m_vpoint1->point()).dist2(centre);
        const Point2D::Real dist1 =  Point2D(m_vpoint2->point()).dist2(centre);
        const Point2D::Real dist2 =  Point2D(m_vpoint3->point()).dist2(centre);

        // choose the closest vanishing point to construct the back wall
        if(dist0 <= std::min(dist1, dist2))
        {
            std::swap(m_vpoint1, m_vpoint3);
        }
        else if(dist1 <= std::min(dist0, dist2))
        {
            std::swap(m_vpoint2, m_vpoint3);
        }
        else // if(dist2 <= std::min(dist0, dist1))
        {
            std::swap(m_vpoint3, m_vpoint3);
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
        // register vanishing points
        if(m_vpoint1)
        {
            selectables.registerObject(this, m_vpoint1);
            m_vpoint1->registerCascade(selectables);
        }
        if(m_vpoint2)
        {
            selectables.registerObject(this, m_vpoint2);
            m_vpoint2->registerCascade(selectables);
        }
        if(m_vpoint3)
        {
            selectables.registerObject(this, m_vpoint3);
            m_vpoint3->registerCascade(selectables);
        }

        // register handle points
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
        if(m_vpoint1) m_vpoint1->render(temp, CV_RGB(255, 0, 0));
        if(m_vpoint2) m_vpoint2->render(temp, CV_RGB(0, 255, 0));
        if(m_vpoint3) m_vpoint3->render(temp, CV_RGB(0, 0, 255));

        // compute corners of the wall
        if(m_vpoint1 && m_vpoint2 && m_vpoint3)
        {
            Point2D corners[4];
            computeCorners(corners, *m_vpoint1, *m_vpoint2);

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
        // save vanishing points
        out << numberOfVanishingPoints() << std::endl;
        if(m_vpoint1) m_vpoint1->save(out);
        if(m_vpoint2) m_vpoint2->save(out);
        if(m_vpoint3) m_vpoint3->save(out);

        if(initialisedEnoughVanishingPoints())
        {
            // save handle points
            out << 4 << std::endl;
            out << m_handles[0] << std::endl;
            out << m_handles[1] << std::endl;
            out << m_handles[2] << std::endl;
            out << m_handles[3] << std::endl;

            // save wall corners
            Point2D corners[4];
            computeCorners(corners, *m_vpoint1, *m_vpoint2);
            out << 4 << std::endl;
            out << corners[0] << std::endl;
            out << corners[1] << std::endl;
            out << corners[2] << std::endl;
            out << corners[3] << std::endl;
        }
        else
        {
            out << 0 << std::endl;
        }
    }

    // input function
    void load(std::ifstream &in)
    {
        // load vanishing points
        int nvanishings = 0;
        in >> nvanishings;
        if(nvanishings > 0)
        {
            m_vpoint1 = new VanishingPoint();
            m_vpoint1->load(in);
        }
        if(nvanishings > 1)
        {
            m_vpoint2 = new VanishingPoint();
            m_vpoint2->load(in);
        }
        if(nvanishings > 2)
        {
            m_vpoint3 = new VanishingPoint();
            m_vpoint3->load(in);
        }

        // load handles
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
        }
    }

private:
    VanishingPoint *m_vpoint1;
    VanishingPoint *m_vpoint2;
    VanishingPoint *m_vpoint3;
    Point2D *m_handles;
};


#endif
