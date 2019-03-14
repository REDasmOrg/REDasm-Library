#ifndef BASE_H
#define BASE_H

#include <functional>
#include "../redasm.h"

#define DO_UNPAREN(...) __VA_ARGS__
#define INVOKE(expr)    expr
#define UNPAREN(args)   INVOKE(DO_UNPAREN args)

#define PLUGIN_NAME(pluginname) public: \
                                static constexpr const char* Name = pluginname; \
                                virtual std::string name() const { return pluginname; } \
                                private:

#define PLUGIN_INHERIT(classname, basename, name, ctor, args) class classname: public UNPAREN(basename) { \
                                                           PLUGIN_NAME(name) \
                                                           public: classname(ctor): UNPAREN(basename)(args) { } \
                                                       };

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
