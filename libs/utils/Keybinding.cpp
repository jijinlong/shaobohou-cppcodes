#include "Keybinding.h"

using std::map;
using std::string;
using std::cout;
using std::endl;

void Keybinding::bind(const string &action, unsigned char key)
{
    map<string, unsigned char>::iterator it = bindings.find(action);
    if(it != bindings.end())
    {
        unsigned char old_key = it->second;
        pressed.erase(old_key);
        reverseBindings.erase(old_key);
        cout << "Unbinded '" << action << "' from '" << (*it).second << "'" << endl;
    }

    bindings[action] = key;
    reverseBindings[key] = action;
    pressed[key] = false;
    cout << "Binded '" << action << "' to '" << key << "'" << endl;
}

bool Keybinding::isActive(bool &active, const string &action) const
{
    map<string, unsigned char>::const_iterator bit = bindings.find(action);
    if(bit == bindings.end())
    {
        cout << "No binding found for <" << action << endl;
        return false;
    }

    unsigned char key = (*bit).second;
    map<unsigned char, bool>::const_iterator pit = pressed.find(key);
    if(pit == pressed.end())
    {
        cout << "Binding for '" << action << "' to '" << key << "' was expected, but not found, very bad." << endl;
        return false;
    }

    active = (*pit).second;
    return true;
}

bool Keybinding::set(unsigned char key, bool flag)
{
    map<unsigned char, bool>::iterator it = pressed.find(key);
    if(it == pressed.end())
    {
//         cout << "'" << key << "' not bound to any action" << endl;
        return false;
    }

    (*it).second = flag;
    return true;
}
