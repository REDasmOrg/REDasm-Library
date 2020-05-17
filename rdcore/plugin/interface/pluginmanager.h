#pragma once

#include <rdapi/plugin.h>
#include <unordered_map>
#include <string>
#include "pluginloader.h"

class PluginManager
{
    public:
        PluginManager();
        ~PluginManager();
        void loadAll(const std::string& pluginpath);
        void unload(const PluginInstance* pi);
        void unloadAll();

    private:
        void load(const std::string& pluginpath);

    private:
        std::unordered_map<library_t, PluginInstance> m_plugins;
};

