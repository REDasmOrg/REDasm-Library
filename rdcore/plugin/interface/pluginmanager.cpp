#include "pluginmanager.h"
#include "pluginloader.h"
#include "../../support/fs.h"

PluginManager::PluginManager() { }
PluginManager::~PluginManager() { this->unloadAll(); }

void PluginManager::loadAll(const std::string& pluginpath)
{
    FS::EntryList entries(FS::recurse(pluginpath));

    for(const FS::Entry& entry : entries)
    {
        if(entry.path.ext() != SHARED_OBJECT_EXT) continue;
        this->load(entry.value());
    }
}

void PluginManager::unload(const RDPluginInstance* pi)
{
    std::string id = pi->descriptor->id;
    PluginLoader::unload(pi);
    m_plugins.erase(id);
}

void PluginManager::unloadAll()
{
    while(!m_plugins.empty())
        this->unload(&m_plugins.begin()->second);
}

void PluginManager::load(const std::string& pluginpath)
{
    RDPluginInstance pi;
    if(!PluginLoader::load(pluginpath, pi)) return;
    m_plugins[pi.descriptor->id] = pi;
}
