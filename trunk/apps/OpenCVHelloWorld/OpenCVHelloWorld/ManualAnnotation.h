#ifndef MANUAL_ANNOTATION_H
#define MANUAL_ANNOTATION_H


#include "VanishingWall.h"


const Point2D outsidePoint(-100, -100);

// line segments
class ManualAnnotation : public Selectable
{
public:
    int width;
    int height;

    ManualAnnotation() 
        : width(0), height(0), currLine(new LineSegment(outsidePoint, outsidePoint)), selectedObject(NULL), m_wall(0)
    {
    }

    ManualAnnotation(int w, int h) 
        : width(w), height(h), currLine(new LineSegment(outsidePoint, outsidePoint)), selectedObject(NULL), m_wall(0)
    {
    }

    ~ManualAnnotation()
    {
        if(currLine) delete currLine;

        for(unsigned int i = 0; i < m_vanishings.size(); i++)
        {
            delete m_vanishings[i];
        }

        if(m_wall) delete m_wall;
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

    void BeginUpdate(int x, int y)
    {
        selectedObject = selectableObjects.selectObject(x, y);

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
                    if(m_vanishings.size() < 3)
                    {
                        m_vanishings.push_back(new VanishingPoint(*currLine));
                        m_vanishings.back()->addLine(LineSegment(Point2D(width-currLine->beg().x(), height-currLine->beg().y()), Point2D(width-currLine->end().x(), height-currLine->end().y())));

                        selectableObjects.registerObject(this, m_vanishings.back());
                        m_vanishings.back()->registerCascade(selectableObjects);

                        std::cout << "ADDED LINE [" << currLine->beg().x() << " " << currLine->beg().y() << "] to [" << currLine->end().x() << " " << currLine->end().y() << "]" << std::endl;
                    }
                    else if(!m_wall)    // add WallBoundary
                    {
                        m_wall = new VanishingWall();
                        m_wall->setup(currLine->beg(), currLine->end(), m_vanishings, width, height);
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
        if(m_vanishings.size()>0) m_vanishings[0]->render(temp, CV_RGB(255, 0, 0));
        if(m_vanishings.size()>1) m_vanishings[1]->render(temp, CV_RGB(0, 255, 0));
        if(m_vanishings.size()>2) m_vanishings[2]->render(temp, CV_RGB(0, 0, 255));

        if(currLine)
        {
            if(m_wall || m_vanishings.size()<3)
            {
                if(m_vanishings.size()==0) currLine->render(temp, CV_RGB(255, 0, 0), 2);
                if(m_vanishings.size()==1) currLine->render(temp, CV_RGB(0, 255, 0), 2);
                if(m_vanishings.size()==2) currLine->render(temp, CV_RGB(0, 0, 255), 2);
            }
            else
            {
                if(!m_wall && m_vanishings.size()>=3)
                {
                    const Point2D::Real Xmin = std::min(currLine->beg().x(), currLine->end().x());
                    const Point2D::Real Xmax = std::max(currLine->beg().x(), currLine->end().x());
                    const Point2D::Real Ymin = std::min(currLine->beg().y(), currLine->end().y());
                    const Point2D::Real Ymax = std::max(currLine->beg().y(), currLine->end().y());

                    Point2D topLeft(Xmin, Ymin);
                    Point2D topRight(Xmax, Ymin);
                    Point2D botRight(Xmax, Ymax);
                    Point2D botLeft(Xmin, Ymax);

                    LineSegment(topLeft,  topRight).render(temp, CV_RGB(255, 0, 0), 2);
                    LineSegment(topRight, botRight).render(temp, CV_RGB(255, 0, 0), 2);
                    LineSegment(botRight, botLeft).render(temp, CV_RGB(255, 0, 0), 2);
                    LineSegment(botLeft,  topLeft).render(temp, CV_RGB(255, 0, 0), 2);
                }
            }
        }

        if(m_wall)
        {
            m_wall->render(temp, CV_RGB(255, 0, 0), 2);
        }
    }

    void save(std::ofstream &out) const
    {
        out << m_vanishings.size() << std::endl;
        for(unsigned int i = 0; i < m_vanishings.size(); i++)
        {
            m_vanishings[i]->save(out);
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

        m_vanishings.clear();
        for(int i = 0; i < nvanishings; i++)
        {
            m_vanishings.push_back(new VanishingPoint());
            m_vanishings[i]->load(in);
        }
        m_wall = new VanishingWall();
        if(m_wall->load(in))
        {
            m_wall->setup(m_vanishings, width, height);
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
        // register vanishings points
        for(unsigned int i = 0; i < m_vanishings.size(); i++)
        {
            selectables.registerObject(this, m_vanishings[i]);
            m_vanishings[i]->registerCascade(selectables);
        }

        // register room geometry
        if(m_wall)
        {
            m_wall->registerCascade(selectables);
        }
    }


private:
    LineSegment *currLine;
    Selectable *selectedObject;
    SelectableGroup selectableObjects;

    std::vector<VanishingPoint*> m_vanishings;
    VanishingWall *m_wall;
};


#endif
