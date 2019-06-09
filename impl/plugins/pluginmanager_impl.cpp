#include "pluginmanager_impl.h"
#include "pluginloader.h"
#include <redasm/support/utils.h>
#include <redasm/context.h>

#ifdef _WIN32
    #include "../libs/dirent/dirent.h"
#else
    #include <dirent.h>
#endif

namespace REDasm {

std::unique_ptr<PluginManager> PluginManagerImpl::m_instance;

void PluginManagerImpl::unload(const PluginInstance *pi)
{
    std::string id = pi->descriptor->id;
    PluginLoader::unload(pi);
    m_activeplugins.erase(id);
}

const PluginInstance *PluginManagerImpl::load(const std::string &pluginpath, const char* initname)
{
    PluginInstance pi;

    if(!PluginLoader::load(pluginpath, initname, &pi))
        return nullptr;

    auto it = m_activeplugins.insert({pi.descriptor->id, pi});
    pi.descriptor->plugin->setInstance(&it.first->second); // Bind Descriptor <-> Plugin
    return &it.first->second;
}

void PluginManagerImpl::iteratePlugins(const char *initname, const PluginManager_Callback &cb)
{
    for(const std::string& pluginpath : r_ctx->pluginPaths())
    {
        DIR* dir = opendir(pluginpath.c_str());

        if(!dir)
            return;

        struct dirent* entry = nullptr;

        while((entry = readdir(dir)))
        {
            if((entry->d_type != DT_REG) || !Utils::endsWith(entry->d_name, SHARED_OBJECT_EXT))
                continue;

            const PluginInstance* pi = nullptr;

            if(!(pi = this->load(Path::create(pluginpath, entry->d_name), initname)))
                continue;

            IterateResult res = cb(pi);

            if(res == IterateResult::Done)
            {
                closedir(dir);
                return;
            }

            if(res == IterateResult::Unload)
                this->unload(pi);
        }

        closedir(dir);
    }
}

void PluginManagerImpl::unloadAll()
{
    while(!m_activeplugins.empty())
    {
        auto it = m_activeplugins.begin();
        this->unload(&it->second);
    }
}

} // namespace REDasm
