#pragma once

#include <functional>
#include <json.hpp>
#include "../redasm.h"

#define PLUGIN_NAME(pluginname) public: \
                                static constexpr const char* Name = pluginname; \
                                std::string name() const override { return pluginname; } \
                                private:

#define PLUGIN_INHERIT(classname, basename, name, ctor, args) class classname: public UNPAREN(basename) { \
                                                           PLUGIN_NAME(name) \
                                                           public: classname(ctor): UNPAREN(basename)(args) { } \
                                                       };

namespace REDasm {

typedef nlohmann::json PluginSettings;

class Plugin
{
    public:
        Plugin() { }
        virtual ~Plugin() { }
        virtual std::string name() const = 0;
        std::string id() const { return m_id; }
        void setId(const std::string& id) { m_id = id; }

    protected:
        PluginSettings m_settings;

    private:
        std::string m_id;
};

}
