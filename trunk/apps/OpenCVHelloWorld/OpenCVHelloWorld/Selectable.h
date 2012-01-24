#ifndef SELECTABLE_H
#define SELECTABLE_H


#include <vector>
#include <algorithm>


// Selectable interface
class Selectable
{
public:
    friend class SelectableGroup;

    Selectable() : m_dim(0) {}

    // computes the selection distance of this Selectable object from (x, y)
    virtual int selection(int x, int y) = 0;

    // update this Selectable object with values (x, y)
    virtual void update(int x, int y) = 0;

    // register this Selectable object (and optionally its sub-components) with a Selectable group
    virtual void registerCascade(SelectableGroup &selectables) = 0;

    // set Selectable pointer selected with this pointer if its selection distance
    // is less than the current best best selection distance bestDist, and is also
    // less than a maximum threshold maxDist
    void selectIfCloser(int x, int y, Selectable *&selected, int &bestDist, const int maxDist)
    {
        const int currDist =  selection(x, y);
        if(currDist <= maxDist)
        {
            if(!selected || m_dim<selected->m_dim || (m_dim==selected->m_dim && currDist<bestDist))
            {
                selected = this;
                bestDist = currDist;
            }
        }
    }

    // update this Selectable object and recursive update its dependant objects
    void updateCascade(int x, int y)
    {
        update(x, y);

        for(unsigned int i = 0; i < m_parents.size(); i++)
        {
            m_parents[i]->updateCascade(x, y);
        }
    }

private:
    int m_dim;
    std::vector<Selectable*> m_parents;     // other Selectable objects that depend on this Selectable object

    Selectable(const Selectable &other);
    Selectable& operator=(const Selectable &other);
};


// Selectable Group
class SelectableGroup
{
public:

    // select the lowest dimensional object with a fixed radius around (x, y).
    // if multiple objects of the same dimensionaity are within the radius,
    // then select the nearest one
    Selectable* selectObject(const int x, const int y, const int radius=10)
    {
        Selectable *selected = 0;

        int bestDist = std::numeric_limits<int>::max();
        for(unsigned int i = 0; i < selectables.size(); i++)
        {
            selectables[i]->selectIfCloser(x, y, selected, bestDist, radius*radius);
        }

        return selected;
    }

    // register selectable parent as a dependant of Selectable child
    void registerObject(Selectable *parent, Selectable *child)
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

private:
    std::vector<Selectable*> selectables;
};


#endif
