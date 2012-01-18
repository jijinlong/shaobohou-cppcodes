// OpenCV_Helloworld.cpp : Defines the entry point for the console application.
// Created for build/install tutorial, Microsoft Visual C++ 2010 Express and OpenCV 2.1.0

//#include "stdafx.h"

#include <Windows.h>

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include <fstream>
#include <iostream>


// see yu2008inferring and hedau2009recovering

const int intmax = std::numeric_limits<int>::max();


// Selectable interface
class Selectable
{
public:
    friend class SelectableGroup;

    Selectable() : m_dim(0), m_parent(0) {}

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

        if(m_parent)
        {
            m_parent->updateCascade(x, y);
        }
    }

private:
    int m_dim;
    Selectable *m_parent;

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

    void addSelectable(Selectable *sel)
    {
        selectables.push_back(sel);
    }

    void registerSelectable(Selectable *parent, Selectable *child)
    {
        if(parent && child)
        {
            child->m_parent = parent;
        }
    }

    std::vector<Selectable*> selectables;
};

SelectableGroup selectableObjects;


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
    LineSegment() : m_beg(0), m_end(0) {}
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

    const Point2D& beg() const
    {
        return *m_beg;
    }

    const Point2D& end() const
    {
        return *m_end;
    }

    Point2D& beg()
    {
        return *m_beg;
    }

    Point2D& end()
    {
        return *m_end;
    }

    void render(IplImage *temp, const CvScalar &col, const int thickness) const
    {
        cvLine(temp, cvPoint(*m_beg), cvPoint(*m_end), col, thickness);
        cvLine(temp, cvPoint(*m_beg), cvPoint(*m_beg), CV_RGB(0, 0, 0), thickness*2);
        cvLine(temp, cvPoint(*m_end), cvPoint(*m_end), CV_RGB(0, 0, 0), thickness*2);
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
    VanishingPoint() {}
    VanishingPoint(const LineSegment &line)
    {
        addLine(line);
    }

    VanishingPoint(const VanishingPoint &)
    {
        assert(false);
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
    }

    void addLine(const LineSegment &line)
    {
        lines.push_back(new LineSegment(line));
    }

    bool computeVanishingPoint(Point2D &vpoint) const
    {
        assert(lines.size()> 1);

        return intersectInfiniteLines(*lines[0], *lines[1], vpoint);
    }

    void render(IplImage *temp, const CvScalar &col) const
    {
        for(unsigned int i = 0; i < lines.size(); i++)
        {
            lines[i]->render(temp, col, 2);
        }

        if(lines.size() > 1)
        {
            Point2D vpoint;
            const bool infinite = computeVanishingPoint(vpoint);

            if(!infinite)
            {
                cvLine(temp, cvPoint(vpoint), cvPoint(vpoint), col, 10);
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

            // setup Selectable hierarchy
            selectableObjects.addSelectable(lines[i]);
            selectableObjects.addSelectable(&lines[i]->beg());
            selectableObjects.addSelectable(&lines[i]->end());
            selectableObjects.registerSelectable(lines[i], &lines[i]->beg());
            selectableObjects.registerSelectable(lines[i], &lines[i]->end());
        }
    }

private:
    std::vector<LineSegment*> lines;
};

// line segments
class Annotation
{
public:
    int width;
    int height;

    Annotation(): width(0), height(0), currLine(NULL), selectedObject(NULL), roomRays(4), roomCorners(4)
    {
        for(unsigned int i = 0; i < roomRays.size();    i++) roomRays[i] = new LineSegment();
        for(unsigned int i = 0; i < roomCorners.size(); i++) roomCorners[i] = new Point2D();
    }

    Annotation(int w, int h) : width(w), height(h), currLine(NULL), selectedObject(NULL), roomRays(4), roomCorners(4)
    {
        for(unsigned int i = 0; i < roomRays.size();    i++) roomRays[i] = new LineSegment();
        for(unsigned int i = 0; i < roomCorners.size(); i++) roomCorners[i] = new Point2D();
    }

    void BeginUpdate(int x, int y)
    {
        selectedObject = selectableObjects.select(x, y);
        //////if(selectedPoint==NULL)
        //////{
        //////    currLine = new LineSegment(Point2D(x, y));
        //////}
        //////else
        //////{
        //////    std::cout << "SELECTED POINT [" << selectedPoint->x() << " " << selectedPoint->y() << "]" << std::endl;
        //////}
    }

    void Update(int x, int y)
    {
        //////if(selectedPoint==NULL)
        //////{
        //////    if(currLine)
        //////    {
        //////        currLine->endPoint = Point2D(x, y);
        //////    }
        //////}
        //////else
        //////{
        //////    selectedPoint->x = x;
        //////    selectedPoint->y = y;
        //////}

        if(selectedObject)
        {
            selectedObject->updateCascade(x, y);
        }

        // compute vanishing points and find the farthest pair
        Point2D vpoints[3];
        vanishings[0]->computeVanishingPoint(vpoints[0]);
        vanishings[1]->computeVanishingPoint(vpoints[1]);
        vanishings[2]->computeVanishingPoint(vpoints[2]);
        const int dist01 = static_cast<int>(vpoints[0].dist2(vpoints[1]));
        const int dist12 = static_cast<int>(vpoints[1].dist2(vpoints[2]));
        const int dist02 = static_cast<int>(vpoints[0].dist2(vpoints[2]));

        Point2D *vpointA = NULL;
        Point2D *vpointB = NULL;
        if(dist01>dist12 && dist01>dist02)
        {
            vpointA = &vpoints[0];
            vpointB = &vpoints[1];
        }
        if(dist12>dist01 && dist12>dist02)
        {
            vpointA = &vpoints[1];
            vpointB = &vpoints[2];
        }
        if(dist02>dist01 && dist02>dist12)
        {
            vpointA = &vpoints[0];
            vpointB = &vpoints[2];
        }

        // recompute room corners
        Point2D centreTopLeft(width/2-100, height/2-100);
        Point2D centreBotRight(width/2+100, height/2+100);
        intersectInfiniteLines(LineSegment(*vpointA, centreTopLeft),  LineSegment(*vpointB, centreTopLeft),  *roomCorners[0]);
        intersectInfiniteLines(LineSegment(*vpointA, centreTopLeft),  LineSegment(*vpointB, centreBotRight), *roomCorners[1]);
        intersectInfiniteLines(LineSegment(*vpointA, centreBotRight), LineSegment(*vpointB, centreBotRight), *roomCorners[2]);
        intersectInfiniteLines(LineSegment(*vpointA, centreBotRight), LineSegment(*vpointB, centreTopLeft),  *roomCorners[3]);
    }

    void EndUpdate(int x, int y)
    {
        //////if(selectedPoint==NULL)
        //////{
        //////    if(currLine)
        //////    {
        //////        if(vanishings.size()<3 && !(currLine->beg().equals(currLine->end())))
        //////        {
        //////            vanishings.push_back(VanishingPoint(*currLine));
        //////            vanishings.back().addLine(LineSegment(Point2D(width-currLine->beg().x(), height-currLine->beg().y()), Point2D(width-currLine->end().x(), height-currLine->end().y())));
        //////            std::cout << "ADDED LINE [" << currLine->beg().x() << " " << currLine->beg().y() << "] to [" << currLine->end().x() << " " << currLine->end().y() << "]" << std::endl;
        //////        }

        //////        delete currLine;
        //////        currLine = NULL;
        //////    }
        //////}
        //////else
        //////{
        //////    selectedPoint->set(x, y);
        //////}

        if(selectedObject)
        {
            selectedObject->updateCascade(x, y);
        }

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

        // find the vanishing point in the image
        Point2D vpoint;
        for(unsigned int i = 0; i < vanishings.size(); i++)
        {
            vanishings[i]->computeVanishingPoint(vpoint);
            if(vpoint.x()>=0 && vpoint.x()<=width && vpoint.y()>=0 && vpoint.y()<=height)
            {
                break;
            }
        }

        const unsigned int ncorners = roomCorners.size();
        for(unsigned int i = 0; i < roomCorners.size(); i++)
        {
            // draw corners
            LineSegment(*roomCorners[i], *roomCorners[i]).render(temp, CV_RGB(255, 0, 0), 4);

            // draw boundaries between side walls and ceilings/floors
            LineSegment(vpoint, *roomCorners[i]+(*roomCorners[i]-vpoint)*10).render(temp, CV_RGB(255, 0, 0), 4);

            // draw back walls
            LineSegment(*roomCorners[i], *(roomCorners[(i+1)%ncorners])).render(temp, CV_RGB(255, 0, 0), 4);
        }
    }

    void save(std::ofstream &out) const
    {
        out << vanishings.size() << std::endl;
        for(unsigned int i = 0; i < vanishings.size(); i++)
        {
            vanishings[i]->save(out);
        }
    }

    void load(std::ifstream &in)
    {
        int nvanishings = 0;
        in >> nvanishings;

        vanishings.clear();
        for(int i = 0; i < nvanishings; i++)
        {
            vanishings.push_back(new VanishingPoint);
            vanishings[i]->load(in);
        }
    }


private:
    LineSegment *currLine;
    Selectable *selectedObject;

    std::vector<VanishingPoint*> vanishings;

    std::vector<LineSegment*> roomRays;
    std::vector<Point2D*> roomCorners;
};

Annotation *annotation = NULL;
bool renderUpdate = false;
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
