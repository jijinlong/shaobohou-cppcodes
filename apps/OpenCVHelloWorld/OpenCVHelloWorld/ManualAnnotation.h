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

    // stub function
    int selection(int x, int y)
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
                    if(vanishings.size() < 3)
                    {
                        vanishings.push_back(new VanishingPoint(*currLine));
                        vanishings.back()->addLine(LineSegment(Point2D(width-currLine->beg().x(), height-currLine->beg().y()), Point2D(width-currLine->end().x(), height-currLine->end().y())));

                        selectableObjects.registerObject(this, vanishings.back());
                        vanishings.back()->registerCascade(selectableObjects);

                        std::cout << "ADDED LINE [" << currLine->beg().x() << " " << currLine->beg().y() << "] to [" << currLine->end().x() << " " << currLine->end().y() << "]" << std::endl;
                    }
                    else if(!m_wall)    // add WallBoundary
                    {
                        m_wall = new VanishingWall();
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
        m_wall = new VanishingWall();
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
            selectables.registerObject(this, vanishings[i]);
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
    VanishingWall *m_wall;
};


#endif
