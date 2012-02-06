#ifndef ANNOTATION_TOOL_H
#define ANNOTATION_TOOL_H


#include "BoxRoomView.h"


const Point2D outsidePoint(-100, -100);

// line segments
class AnnotationTool : public Selectable
{
public:
    AnnotationTool() 
        : currLine(new LineSegment(outsidePoint, outsidePoint)), selectedObject(NULL)
    {
    }

    ~AnnotationTool()
    {
        if(currLine) delete currLine;
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

    void initBoxRoomView(const int w, const int h)
    {
        m_wall = new BoxRoomView(w, h);
    }

    void BeginUpdate(int x, int y)
    {
        selectedObject = selectableObjects.selectObject(x, y);

        // initialise a line segment
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
                    if(m_wall->initialisedEnoughVanishingPoints())
                    {
                        m_wall->initialiseRoomGeometry(currLine->beg(), currLine->end());
                        m_wall->registerCascade(selectableObjects);
                    }
                    else
                    {
                        VanishingPoint *newPoint = m_wall->initialiseNewVanishingPoint(*currLine);
                        newPoint->registerCascade(selectableObjects);

                        std::cout << "ADDED LINE [" << currLine->beg().x() << " " << currLine->beg().y() << "] to [" << currLine->end().x() << " " << currLine->end().y() << "]" << std::endl;
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
        if(currLine)
        {
            if(m_wall->initialisedEnoughVanishingPoints())
            {
                const Real Xmin = std::min(currLine->beg().x(), currLine->end().x());
                const Real Xmax = std::max(currLine->beg().x(), currLine->end().x());
                const Real Ymin = std::min(currLine->beg().y(), currLine->end().y());
                const Real Ymax = std::max(currLine->beg().y(), currLine->end().y());

                Point2D topLeft( Xmin, Ymin);
                Point2D topRight(Xmax, Ymin);
                Point2D botRight(Xmax, Ymax);
                Point2D botLeft( Xmin, Ymax);

                LineSegment(topLeft,  topRight).render(temp, CV_RGB(255, 0, 0), 2);
                LineSegment(topRight, botRight).render(temp, CV_RGB(255, 0, 0), 2);
                LineSegment(botRight, botLeft ).render(temp, CV_RGB(255, 0, 0), 2);
                LineSegment(botLeft,  topLeft ).render(temp, CV_RGB(255, 0, 0), 2);
            }
            else
            {
                if(m_wall->numberOfVanishingPoints()==0) currLine->render(temp, CV_RGB(255, 0, 0), 2);
                if(m_wall->numberOfVanishingPoints()==1) currLine->render(temp, CV_RGB(0, 255, 0), 2);
                if(m_wall->numberOfVanishingPoints()==2) currLine->render(temp, CV_RGB(0, 0, 255), 2);
            }
        }

        if(m_wall)
        {
            m_wall->render(temp, CV_RGB(255, 0, 0), 2);
        }
    }

    void save(std::ofstream &out) const
    {
        m_wall->save(out);
    }

    void load(std::ifstream &in)
    {
        m_wall->load(in);

        // register components
        registerCascade(selectableObjects);
    }

    void registerCascade(SelectableGroup &selectables)
    {
        // register room geometry
        if(m_wall)
        {
            m_wall->registerCascade(selectables);
        }
    }


private:
    LineSegment *currLine;
    BoxRoomView *m_wall;

    Selectable *selectedObject;
    SelectableGroup selectableObjects;
};


#endif
