#pragma once

#include <rdapi/plugin.h>
#include <unordered_map>
#include <string>

class PluginManager
{
    public:
        PluginManager();
        ~PluginManager();
        void loadAll(const std::string& pluginpath);
        void unload(const RDPluginInstance* pi);
        void unloadAll();

    private:
        void load(const std::string& pluginpath);

    private:
        std::unordered_map<std::string, RDPluginInstance> m_plugins;
};

