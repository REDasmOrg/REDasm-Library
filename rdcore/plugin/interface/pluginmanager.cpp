#include "pluginmanager.h"
#include <filesystem>

PluginManager::PluginManager() { }
PluginManager::~PluginManager() { this->unloadAll(); }

void PluginManager::loadAll(const std::string& pluginpath)
{
    std::filesystem::directory_entry e(pluginpath);
    if(!e.is_directory()) return;

    for(const auto& entry : std::filesystem::recursive_directory_iterator(e, std::filesystem::directory_options::follow_directory_symlink))
    {
        if(entry.is_directory() || entry.path().extension() != SHARED_OBJECT_EXT) continue;
        this->load(entry.path());
    }
}

void PluginManager::unload(const PluginInstance* pi)
{
    library_t handle = pi->handle;
    PluginLoader::unload(pi);

    m_plugins.erase(handle);
}

void PluginManager::unloadAll() { while(!m_plugins.empty()) this->unload(&m_plugins.begin()->second); }

void PluginManager::load(const std::string& pluginpath)
{
    PluginInstance pi;
    if(!PluginLoader::load(pluginpath, pi)) return;
    m_plugins[pi.handle] = pi;
}
