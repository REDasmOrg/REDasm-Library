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

    // Bind Descriptor <-> Plugin
    pi.descriptor->plugin->setDescriptor(pi.descriptor);
    auto it = m_activeplugins.insert({pi.descriptor->id, pi});
    return &it.first->second;
}

void PluginManagerImpl::iteratePlugins(const char *initname, const PluginManager_Callback &cb)
{
    this->unloadPlugins();
    DIR* dir = opendir(r_ctx->pluginPath().c_str());

    if(!dir)
    {
        r_ctx->log("Cannot load plugins from " + Utils::quoted(r_ctx->pluginPath()));
        return;
    }

    struct dirent* entry = nullptr;

    while((entry = readdir(dir)))
    {
        if((entry->d_type != DT_REG) || !Utils::endsWith(entry->d_name, SHARED_OBJECT_EXT))
            continue;

        const PluginInstance* pi = nullptr;

        if(!(pi = this->load(Path::create(r_ctx->pluginPath(), entry->d_name), initname)))
            continue;

        IterateResult res = cb(pi);

        if(res == IterateResult::Done)
            break;
        if(res == IterateResult::Unload)
            this->unload(pi);
    }

    closedir(dir);
}

void PluginManagerImpl::unloadPlugins()
{
    while(!m_activeplugins.empty())
    {
        auto it = m_activeplugins.begin();
        this->unload(&it->second);
    }
}

} // namespace REDasm
