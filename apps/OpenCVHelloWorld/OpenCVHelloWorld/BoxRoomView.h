#ifndef BOX_ROOM_VIEW_H
#define BOX_ROOM_VIEW_H


#include "VanishingPoint.h"
#include "Macros.h"


// projective view of a box room defined by three vanishing points
class BoxRoomView : public Selectable
{
public:
    static const int MAX_VPOINTS = 3;
    static const int MAX_HANDLES = 4;
    static const int MAX_CORNERS = 4;

    BoxRoomView(const int w, const int h) : m_width(w), m_height(h)
    {
        FORLOOP(i, MAX_HANDLES)
        {
            m_handles.push_back(new Point2D());
        }

        FORLOOP(i, MAX_CORNERS)
        {
            m_corners.push_back(new Point2D());
        }
    }

    ~BoxRoomView()
    {
        FORLOOP(i, m_vpoints.size())
        {
            delete m_vpoints[i];
        }

        FORLOOP(i, m_handles.size())
        {
            delete m_handles[i];
        }

        FORLOOP(i, m_corners.size())
        {
            delete m_corners[i];
        }
    }

    // has all three vanishing points been initialised
    int numberOfVanishingPoints() const
    {
        return static_cast<int>(m_vpoints.size());
    }

    // has all three vanishing points been initialised
    bool initialisedEnoughVanishingPoints() const
    {
        return numberOfVanishingPoints() >= 3;
    }

    // initialise a new vanishing point if more needed
    VanishingPoint* initialiseNewVanishingPoint(const LineSegment &line)
    {
        VanishingPoint *vpoint = 0;

        // initialise a new vanishing point
        if(vpoint)
        {
            vpoint = new VanishingPoint(line);
            vpoint->addLine(LineSegment(Point2D(line.beg())+Point2D(10, 10), Point2D(line.end())+Point2D(10, 10)));
            m_vpoints.push_back(vpoint);
        }
        
        return vpoint;
    }

    // setup function
    void setup(const Point2D &first, const Point2D &second)
    {
        // compute the bounding box
        const Real Xmin = std::min(first.x(), second.x());
        const Real Xmax = std::max(first.x(), second.x());
        const Real Ymin = std::min(first.y(), second.y());
        const Real Ymax = std::max(first.y(), second.y());

        // initialise control points in clockwise winding
        m_handles[0]->set(Point2D((Xmin+Xmax)/2, Ymin));
        m_handles[1]->set(Point2D(Xmax, (Ymin+Ymax)/2));
        m_handles[2]->set(Point2D((Xmin+Xmax)/2, Ymax));
        m_handles[3]->set(Point2D(Xmin, (Ymin+Ymax)/2));

        // call the real setup function
        setup();
    }

    // setup function
    void setup()
    {
        assert(initialisedEnoughVanishingPoints());

        // find the vanishing point closest to the centre of the image
        const Point2D centre(m_width/2, m_height/2);
        VanishingPoint **insidePoint = &m_vpoints[0];
        FORLOOP(i, m_vpoints.size())
        {
            const Real oldDist = Point2D((*insidePoint)->point()).dist2(centre);
            const Real newDist = Point2D(  m_vpoints[i]->point()).dist2(centre);

            if(newDist <= oldDist)
            {
                insidePoint = &m_vpoints[i];
            }
        }
        std::swap(*insidePoint, m_vpoints.back());   // move the inside vanishing point to the back

        // swap vanishing points if necessary, the correct order 
        // should form a quadrilaterial with longer edges
        computeCorners(*m_vpoints[0], *m_vpoints[1]);
        const Real len1 = m_corners[0]->dist2(*m_corners[1]);
        computeCorners(*m_vpoints[1], *m_vpoints[0]);
        const Real len2 = m_corners[0]->dist2(*m_corners[1]);
        if(len2 > len1)
        {
            std::swap(m_vpoints[0], m_vpoints[1]);
        }

        // recompute the handle points
        computeCorners(*m_vpoints[0], *m_vpoints[1]);
        m_handles[0]->set((*m_corners[3]+*m_corners[0])/2);
        m_handles[1]->set((*m_corners[0]+*m_corners[1])/2);
        m_handles[2]->set((*m_corners[1]+*m_corners[2])/2);
        m_handles[3]->set((*m_corners[2]+*m_corners[3])/2);
    }

    // stub function
    int select(int x, int y)
    {
        return std::numeric_limits<int>::max();
    }

    // stub function
    void update(int x, int y)
    {
        computeCorners(*m_vpoints[0], *m_vpoints[1]);
    }

    // register function
    void registerCascade(SelectableGroup &selectables)
    {
        // register vanishing points
        FORLOOP(i, m_vpoints.size())
        {
            selectables.registerObject(this, m_vpoints[i]);
            m_vpoints[i]->registerCascade(selectables);
        }

        // register handle points
        FORLOOP(i, m_handles.size())
        {
            selectables.registerObject(this, m_handles[i]);
        }
    }

    // render function
    void render(IplImage *temp, const CvScalar &col, const int thickness) const
    {
        if(m_vpoints.size()>0) m_vpoints[0]->render(temp, CV_RGB(255, 0, 0));
        if(m_vpoints.size()>1) m_vpoints[1]->render(temp, CV_RGB(0, 255, 0));
        if(m_vpoints.size()>2) m_vpoints[2]->render(temp, CV_RGB(0, 0, 255));

        // compute corners of the wall
        if(initialisedEnoughVanishingPoints())
        {
            // draw control points
            cvLine(temp, cvPoint(*m_handles[0]), cvPoint(*m_handles[0]), CV_RGB(0, 0, 0), thickness*5);
            cvLine(temp, cvPoint(*m_handles[1]), cvPoint(*m_handles[1]), CV_RGB(0, 0, 0), thickness*5);
            cvLine(temp, cvPoint(*m_handles[2]), cvPoint(*m_handles[2]), CV_RGB(0, 0, 0), thickness*5);
            cvLine(temp, cvPoint(*m_handles[3]), cvPoint(*m_handles[3]), CV_RGB(0, 0, 0), thickness*5);

            // draw wall edges
            cvLine(temp, cvPoint(*m_corners[0]), cvPoint(*m_corners[1]), col, thickness);
            cvLine(temp, cvPoint(*m_corners[1]), cvPoint(*m_corners[2]), col, thickness);
            cvLine(temp, cvPoint(*m_corners[2]), cvPoint(*m_corners[3]), col, thickness);
            cvLine(temp, cvPoint(*m_corners[3]), cvPoint(*m_corners[0]), col, thickness);

            // draw the edges between walls and floors/ceilings
            cvLine(temp, cvPoint(*m_corners[0]+(*m_corners[0]-Point2D(m_vpoints[2]->point()))*10), cvPoint(*m_corners[0]), col, thickness);
            cvLine(temp, cvPoint(*m_corners[1]+(*m_corners[1]-Point2D(m_vpoints[2]->point()))*10), cvPoint(*m_corners[1]), col, thickness);
            cvLine(temp, cvPoint(*m_corners[2]+(*m_corners[2]-Point2D(m_vpoints[2]->point()))*10), cvPoint(*m_corners[2]), col, thickness);
            cvLine(temp, cvPoint(*m_corners[3]+(*m_corners[3]-Point2D(m_vpoints[2]->point()))*10), cvPoint(*m_corners[3]), col, thickness);
        }
    }

    // output function
    void save(std::ofstream &out) const
    {
        // save vanishing points
        out << m_vpoints.size() << std::endl;
        FORLOOP(i, m_vpoints.size())
        {
            m_vpoints[i]->save(out);
        }

        if(initialisedEnoughVanishingPoints())
        {
            // save handle points
            out << m_handles.size() << std::endl;
            FORLOOP(i, m_handles.size())
            {
                out << *m_handles[i] << std::endl;
            }

            // save wall corners
            out << 4 << std::endl;
            FORLOOP(i, 4)
            {
                out << *m_corners[i] << std::endl;
            }
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
        FORLOOP(i, nvanishings)
        {
            m_vpoints.push_back(new VanishingPoint());
            m_vpoints.back()->load(in);
        }

        // load handle points
        int nhandles = 0;
        in >> nhandles;
        if(nhandles==MAX_HANDLES)
        {
            FORLOOP(i, nhandles)
            {
                Point2D temp;
                in >> temp;
                m_handles[i]->set(temp);
            }
        }
    }

private:
    int m_width, m_height;
    std::vector<VanishingPoint*> m_vpoints;
    std::vector<Point2D*> m_handles;
    std::vector<Point2D*> m_corners;

    // recompute the corners of the back wall;
    void computeCorners(const VanishingPoint &vpoint1, const VanishingPoint &vpoint2) const
    {
        // cast two rays from each vanishing point outside of the image
        LineSegment hline0(Point2D(vpoint1.point()), *m_handles[0]);
        LineSegment hline2(Point2D(vpoint1.point()), *m_handles[2]);
        LineSegment vline1(Point2D(vpoint2.point()), *m_handles[1]);
        LineSegment vline3(Point2D(vpoint2.point()), *m_handles[3]);

        // intersect rays from different vanishing points to form corners
        m_corners[0]->set(Point2D(intersectInfiniteLines(hline0, vline1)));
        m_corners[1]->set(Point2D(intersectInfiniteLines(hline2, vline1)));
        m_corners[2]->set(Point2D(intersectInfiniteLines(hline2, vline3)));
        m_corners[3]->set(Point2D(intersectInfiniteLines(hline0, vline3)));
    }
};


#endif
