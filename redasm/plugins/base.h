#ifndef BASE_H
#define BASE_H

#include <functional>
#include "../redasm.h"

namespace REDasm {

class Plugin
{
    public:
        Plugin() { }
        virtual ~Plugin() { }
        virtual std::string name() const = 0;
};

}

#endif // BASE_H
