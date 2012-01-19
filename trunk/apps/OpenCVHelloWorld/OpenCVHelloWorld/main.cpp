// OpenCV_Helloworld.cpp : Defines the entry point for the console application.
// Created for build/install tutorial, Microsoft Visual C++ 2010 Express and OpenCV 2.1.0

//#include "stdafx.h"

#include <Windows.h>

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include <fstream>
#include <iostream>
#include <cassert>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif


// see yu2008inferring and hedau2009recovering

const int intmax = std::numeric_limits<int>::max();


// pre-declare
class SelectableGroup;

// Selectable interface
class Selectable
{
public:
    friend class SelectableGroup;

    Selectable() : m_dim(0) {}

    // return true if the selection distance of the current instance is smaller than bestDist
    virtual int selectionDistance(int x, int y) = 0;

    void select(int x, int y, Selectable *&selected, int &bestDist, const int maxDist)
    {
        const int currDist =  selectionDistance(x, y);
        if(currDist <= maxDist)
        {
            if(!selected || m_dim<selected->m_dim || (m_dim==selected->m_dim && currDist<bestDist))
            {
                selected = this;
                bestDist = currDist;
            }
        }
    }

    // update the selected instance
    virtual void update(int x, int y) = 0;

    void updateCascade(int x, int y)
    {
        update(x, y);

        for(unsigned int i = 0; i < m_parents.size(); i++)
        {
            m_parents[i]->updateCascade(x, y);
        }
    }

    virtual void registerCascade(SelectableGroup &selectables) = 0;

private:
    int m_dim;
    std::vector<Selectable*> m_parents;

    Selectable(const Selectable &other);
    Selectable& operator=(const Selectable &other);
};

// Selectable Group
class SelectableGroup
{
public:
    Selectable* select(const int x, const int y, const int radius=10)
    {
        Selectable *selected = 0;

        int bestDist = intmax;
        for(unsigned int i = 0; i < selectables.size(); i++)
        {
            selectables[i]->select(x, y, selected, bestDist, radius*radius);
        }

        return selected;
    }

    void registerSelectable(Selectable *parent, Selectable *child)
    {
        if(parent && child)
        {
            child->m_parents.push_back(parent);

            // insert parent if absent
            if(std::find(selectables.begin(), selectables.end(), parent)==selectables.end())
            {
                selectables.push_back(parent);
            }

            // insert child if absent
            if(std::find(selectables.begin(), selectables.end(), child)==selectables.end())
            {
                selectables.push_back(child);
            }
        }
    }

    std::vector<Selectable*> selectables;
};

// Point Functions
class Point2D : public Selectable
{
public:
    typedef double Real;

    friend CvPoint cvPoint(const Point2D &other);

    Point2D() : m_x(0), m_y(0) {}
    Point2D(const Real x, const Real y) : m_x(x), m_y(y) {}
    Point2D(const Point2D &other) : m_x(other.m_x), m_y(other.m_y) {}

    int selectionDistance(int x, int y)
    {
        return static_cast<int>(dist2(Point2D(x, y)));
    }

    void update(int x, int y)
    {
        set(x, y);
    }

    void registerCascade(SelectableGroup &selectables)
    {
    }

    const Real& x() const
    {
        return m_x;
    }

    const Real& y() const
    {
        return m_y;
    }

    void set(int x, int y)
    {
        m_x = x;
        m_y = y;
    }

    void set(const Point2D &other)
    {
        m_x = other.m_x;
        m_y = other.m_y;
    }

    bool equals(const Point2D &other) const
    {
        return this->m_x==other.m_x && this->m_y==other.m_y;
    }

    Real dist2(const Point2D &other) const
    {
        const Real dx = this->m_x - other.m_x;
        const Real dy = this->m_y - other.m_y;
        return dx*dx + dy*dy;
    }

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

private:
    Real m_x, m_y;
};

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
    int selectionDistance(int x, int y)
    {
        return intmax;
    }

    // stub function
    void update(int x, int y)
    {
    }

    void registerCascade(SelectableGroup &selectables)
    {
        selectables.registerSelectable(this, m_beg);
        m_beg->registerCascade(selectables);

        selectables.registerSelectable(this, m_end);
        m_end->registerCascade(selectables);
    }

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

    void render(IplImage *temp, const CvScalar &col, const int thickness) const
    {
        cvLine(temp, cvPoint(*m_beg), cvPoint(*m_end), col, thickness);
        cvLine(temp, cvPoint(*m_beg), cvPoint(*m_beg), CV_RGB(0, 0, 0), thickness*5);
        cvLine(temp, cvPoint(*m_end), cvPoint(*m_end), CV_RGB(0, 0, 0), thickness*5);
    }

private:
    Point2D *m_beg;
    Point2D *m_end;
};

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

        vpoint.set(static_cast<int>(x1+u1*(x2-x1)), static_cast<int>(y1+u1*(y2-y1)));

        return false;
    }
}

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
        for(unsigned int i = 0; i < lines.size(); i++)
        {
            delete lines[i];
        }
    }

    // stub function
    int selectionDistance(int x, int y)
    {
        return intmax;
    }

    // stub function
    void update(int x, int y)
    {
        assert(lines.size()> 1);

        // update vanishing point
        m_atInfinity = intersectInfiniteLines(*lines[0], *lines[1], *m_point);
    }

    void registerCascade(SelectableGroup &selectables)
    {
        for(unsigned int i = 0; i < lines.size(); i++)
        {
            selectables.registerSelectable(this, lines[i]);
            lines[i]->registerCascade(selectables);
        }
    }

    void addLine(const LineSegment &line)
    {
        lines.push_back(new LineSegment(line));
    }

    Point2D* point() const
    {
        return m_point;
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

// Wall Boundary
class Wall : public Selectable
{
public:
    Wall() : m_handles(new Point2D[4]), m_vpoint1(0), m_vpoint2(0), m_vpoint3(0) {}

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
    int selectionDistance(int x, int y)
    {
        return intmax;
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

const Point2D outsidePoint(-100, -100);

// line segments
class Annotation : public Selectable
{
public:
    int width;
    int height;

    Annotation() 
        : width(0), height(0), currLine(new LineSegment(outsidePoint, outsidePoint)), selectedObject(NULL), m_wall(0)
    {
    }

    Annotation(int w, int h) 
        : width(w), height(h), currLine(new LineSegment(outsidePoint, outsidePoint)), selectedObject(NULL), m_wall(0)
    {
    }

    // stub function
    int selectionDistance(int x, int y)
    {
        return intmax;
    }

    // stub function
    void update(int x, int y)
    {
    }

    void BeginUpdate(int x, int y)
    {
        selectedObject = selectableObjects.select(x, y);

        // 
        if(!selectedObject)
        {
            currLine->setBeg(Point2D(x, y));
            currLine->setEnd(Point2D(x, y));
        }
    }

    void Update(int x, int y)
    {
        if(selectedObject)
        {
            selectedObject->updateCascade(x, y);
        }
        else
        {
            if(currLine->beg().x()>=0 && currLine->beg().x()>=0)
            {
                currLine->setEnd(Point2D(x,y));
            }
        }
    }

    void EndUpdate(int x, int y)
    {
        // cascade update
        if(selectedObject)
        {
            selectedObject->updateCascade(x, y);
        }
        else
        {
            if(currLine->beg().x()>=0 && currLine->beg().x()>=0)
            {
                currLine->setEnd(Point2D(x,y));

                if(currLine->beg().equals(currLine->end()))
                {

                }
                else
                {
                    // create a new vanishing point if not enough exit
                    if(vanishings.size() < 3)
                    {
                        vanishings.push_back(new VanishingPoint(*currLine));
                        vanishings.back()->addLine(LineSegment(Point2D(width-currLine->beg().x(), height-currLine->beg().y()), Point2D(width-currLine->end().x(), height-currLine->end().y())));

                        selectableObjects.registerSelectable(this, vanishings.back());
                        vanishings.back()->registerCascade(selectableObjects);

                        std::cout << "ADDED LINE [" << currLine->beg().x() << " " << currLine->beg().y() << "] to [" << currLine->end().x() << " " << currLine->end().y() << "]" << std::endl;
                    }
                    else if(!m_wall)    // add WallBoundary
                    {
                        m_wall = new Wall();
                        const Point2D::Real dist0 =  vanishings[0]->point()->dist2(Point2D(width/2, height/2));
                        const Point2D::Real dist1 =  vanishings[1]->point()->dist2(Point2D(width/2, height/2));
                        const Point2D::Real dist2 =  vanishings[2]->point()->dist2(Point2D(width/2, height/2));

                        if(dist0 <= std::min(dist1, dist2))
                        {
                            m_wall->setup(currLine->beg(), currLine->end(), vanishings[1], vanishings[2], vanishings[0]);
                        }
                        else if(dist1 <= std::min(dist0, dist2))
                        {
                            m_wall->setup(currLine->beg(), currLine->end(), vanishings[0], vanishings[2], vanishings[1]);
                        }
                        else // if(dist2 <= std::min(dist0, dist1))
                        {
                            m_wall->setup(currLine->beg(), currLine->end(), vanishings[0], vanishings[1], vanishings[2]);
                        }

                        m_wall->registerCascade(selectableObjects);
                    }
                }
            }
        }

        currLine->setBeg(outsidePoint);
        currLine->setEnd(outsidePoint);
        selectedObject = NULL;
    }

    void render(IplImage *temp) const
    {
        if(vanishings.size()>0) vanishings[0]->render(temp, CV_RGB(255, 0, 0));
        if(vanishings.size()>1) vanishings[1]->render(temp, CV_RGB(0, 255, 0));
        if(vanishings.size()>2) vanishings[2]->render(temp, CV_RGB(0, 0, 255));

        if(currLine)
        {
            if(vanishings.size()==0) currLine->render(temp, CV_RGB(255, 0, 0), 2);
            if(vanishings.size()==1) currLine->render(temp, CV_RGB(0, 255, 0), 2);
            if(vanishings.size()==2) currLine->render(temp, CV_RGB(0, 0, 255), 2);
        }

        if(m_wall)
        {
            m_wall->render(temp, CV_RGB(255, 0, 0), 2);
        }

        // find the vanishing point in the image
        Point2D vpoint;
        for(unsigned int i = 0; i < vanishings.size(); i++)
        {
            vpoint.set(*vanishings[i]->point());
            if(vpoint.x()>=0 && vpoint.x()<=width && vpoint.y()>=0 && vpoint.y()<=height)
            {
                break;
            }
        }
    }

    void save(std::ofstream &out) const
    {
        out << vanishings.size() << std::endl;
        for(unsigned int i = 0; i < vanishings.size(); i++)
        {
            vanishings[i]->save(out);
        }
        if(m_wall)
        {
            m_wall->save(out);
        }
        else
        {
            out << 0 << std::endl;
        }
    }

    void load(std::ifstream &in)
    {
        int nvanishings = 0;
        in >> nvanishings;

        vanishings.clear();
        for(int i = 0; i < nvanishings; i++)
        {
            vanishings.push_back(new VanishingPoint());
            vanishings[i]->load(in);
        }
        m_wall = new Wall();
        if(m_wall->load(in))
        {
            m_wall->setup(vanishings, width, height);
        }
        else
        {
            delete m_wall;
            m_wall = 0;
        }

        // register components
        registerCascade(selectableObjects);
    }

    void registerCascade(SelectableGroup &selectables)
    {
        // register all components
        for(unsigned int i = 0; i < vanishings.size(); i++)
        {
            selectables.registerSelectable(this, vanishings[i]);
            vanishings[i]->registerCascade(selectables);
        }
        if(m_wall)
        {
            m_wall->registerCascade(selectables);
        }
    }


private:
    LineSegment *currLine;
    Selectable *selectedObject;
    SelectableGroup selectableObjects;

    std::vector<VanishingPoint*> vanishings;
    Wall *m_wall;

    //std::vector<LineSegment*> roomRays;
    //std::vector<Point2D*> roomCorners;
};

bool renderUpdate = false;
Annotation *annotation = NULL;
void MouseCallback(int event, int x, int y, int flags, void* param)
{
    if(annotation)
    {
        switch(event)
        {
        case CV_EVENT_MOUSEMOVE:
            annotation->Update(x, y);
            renderUpdate = true;
            break;

        case CV_EVENT_LBUTTONDOWN:
            annotation->BeginUpdate(x, y);
            break;

        case CV_EVENT_LBUTTONUP:
            annotation->EndUpdate(x, y);
            renderUpdate = true;
            break;
        }
    }
}

//int g_switch_value = 0;
//void SwitchCallback(int position)
//{
//	const int bah = 0;
//}

//// Returns an empty string if dialog is canceled
//std::string openfilename(char *filter = "All Files (*.*)\0*.*\0", HWND owner = NULL) 
//{
//    OPENFILENAME ofn;
//    char fileName[MAX_PATH] = "";
//    ZeroMemory(&ofn, sizeof(ofn));
//
//    ofn.lStructSize = sizeof(OPENFILENAME);
//    ofn.hwndOwner = owner;
//    ofn.lpstrFilter = filter;
//    ofn.lpstrFile = fileName;
//    ofn.nMaxFile = MAX_PATH;
//    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
//    ofn.lpstrDefExt = "";
//
//    std::string fileNameStr;
//
//    if ( GetOpenFileName(&ofn) )
//        fileNameStr = fileName;
//
//    return fileNameStr;
//}

//int _tmain(int argc, _TCHAR* argv[])
int main(int argc, char *argv[])
{
    std::string windowName = "Annotation";
    std::string imageName = "0000000004.jpg";
    IplImage *img = cvLoadImage(imageName.c_str());
    IplImage *temp = cvCloneImage(img);


    // load annotation from file if exists
    annotation = new Annotation(img->width, img->height);
    std::ifstream fin(imageName+".dat");
    if(fin.is_open())
    {
        annotation->load(fin);
        renderUpdate = true;
    }

    cvNamedWindow(windowName.c_str(), 1);
    cvShowImage(windowName.c_str(), img);

    cvSetMouseCallback(windowName.c_str(), MouseCallback);
    //cvCreateTrackbar("Switch", windowName.c_str(), &g_switch_value, 1, switch_callback);

    while(true)
    {
        if(renderUpdate)
        {
            cvCopyImage(img, temp);
            annotation->render(temp);
            cvShowImage(windowName.c_str(), temp);

            renderUpdate = false;
        }

        if(cvWaitKey(15)==27) 
            break;
    }

    //        cvWaitKey();
    cvDestroyWindow(windowName.c_str());
    cvReleaseImage(&img);

    // save annotation to file
    std::ofstream fout(imageName+".dat");
    annotation->save(fout);
    fout.close();

    delete annotation;

    return 0;
}
