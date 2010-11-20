#ifndef KEYBINDING_H
#define KEYBINDING_H

#include <vector>
#include <string>
#include <map>
#include <iostream>

class Keybinding
{
    public:
        Keybinding(){};
        virtual ~Keybinding(){};

        void bind(const std::string &action, unsigned char key);
        bool isActive(bool &active, const std::string &action) const;
        virtual bool set(unsigned char key, bool flag);

    protected:
        std::map<std::string, unsigned char> bindings;
        std::map<unsigned char, std::string> reverseBindings;
        std::map<unsigned char, bool> pressed;
};

#endif
