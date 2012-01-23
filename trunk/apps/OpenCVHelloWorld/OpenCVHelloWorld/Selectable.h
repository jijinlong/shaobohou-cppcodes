#ifndef SELECTABLE_H
#define SELECTABLE_H


#include <vector>
#include <algorithm>


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

        int bestDist = std::numeric_limits<int>::max();
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


#endif
