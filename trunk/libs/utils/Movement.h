#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "Keybinding.h"

class Movement : public Keybinding
{
    public:
        bool upward, downward, forward, backward, leftward, rightward;
        bool rotXPos, rotXNeg, rotYPos, rotYNeg, rotZPos, rotZNeg;

        Movement() : upward(false), downward(false), forward(false), backward(false), leftward(false), rightward(false),
                     rotXPos(false), rotXNeg(false), rotYPos(false), rotYNeg(false), rotZPos(false), rotZNeg(false)
        {
            bind("rotXPos", '2');   bind("rotXNeg", '8');
            bind("rotYPos", '6');   bind("rotYNeg", '4');
            bind("rotZPos", '[');   bind("rotZNeg", ']');
            bind("upward", 'q');    bind("downward", 'e');
            bind("forward", 'w');   bind("backward", 's');
            bind("leftward", 'a');  bind("rightward", 'd');
            rebind();
        };

        Movement(const Movement &other) : Keybinding(other)
        {
            rebind();
        };

        Movement& operator=(const Movement &other)
        {
            Keybinding::operator=(other);
            rebind();
            return *this;
        };

        virtual ~Movement(){};

        virtual bool set(unsigned char key, bool flag)
        {
            bool success = Keybinding::set(key, flag);
            if(success)
            {
                std::map<unsigned char, std::string>::iterator rit = reverseBindings.find(key);
                if(rit == reverseBindings.end())
                {
                    std::cout << "No reverse binding for '" << key << "', very bad" << std::endl;
                    return false;
                }

                const std::string &actionName = (*rit).second;
                std::map<std::string, bool*>::iterator cit = cache.find(actionName);
                if(cit == cache.end())
                {
                    std::cout << "'" << actionName << "' not bound to any bool* " << std::endl;
                    return false;
                }

                (*((*cit).second)) = flag;
            }

            return success;
        }

    private:
        std::map<std::string, bool*> cache;

        void rebind()
        {
            cache["rotXPos"] = &rotXPos;    cache["rotXNeg"] = &rotXNeg;
            cache["rotYPos"] = &rotYPos;    cache["rotYNeg"] = &rotYNeg;
            cache["rotZPos"] = &rotZPos;    cache["rotZNeg"] = &rotZNeg;
            cache["upward"] = &upward;      cache["downward"] = &downward;
            cache["forward"] = &forward;    cache["backward"] = &backward;
            cache["leftward"] = &leftward;  cache["rightward"] = &rightward;
        };
};

#endif
