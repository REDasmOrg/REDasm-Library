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

void PluginListImpl::weightSort()
{
    std::sort(m_list.begin(), m_list.end(), [](const PluginInstance* pi1, const PluginInstance* pi2) -> bool {
        return pi1->descriptor->plugin->weight() < pi2->descriptor->plugin->weight();
    });
}

void PluginListImpl::alphaSort()
{
    std::sort(m_list.begin(), m_list.end(), [](const PluginInstance* pi1, const PluginInstance* pi2) -> bool {
        return String(pi1->descriptor->description) < String(pi2->descriptor->description);
    });
}

void PluginManagerImpl::unload(const PluginInstance *pi)
{
    String id = pi->descriptor->id;
    PluginLoader::unload(pi);
    m_activeplugins.pimpl_p()->erase(id);
}

PluginManagerImpl::PluginManagerImpl()
{
    this->loadResidentPlugins();
}

const PluginInstance *PluginManagerImpl::load(const String &pluginpath, const String &initname)
{
    PluginInstance pi;
    if(!PluginLoader::load(pluginpath, initname, &pi)) return nullptr;

    this->pushActive(&pi);
    return &m_activeplugins.pimpl_p()->value(pi.descriptor->id);
}

void PluginManagerImpl::iteratePlugins(const String &initname, const PluginManager_Callback &cb)
{
    for(const String& pluginpath : r_ctx->pluginPaths())
    {
        if(this->iteratePlugins(pluginpath.c_str(), initname, cb))
            break;
    }
}

void PluginManagerImpl::unloadAll()
{
    while(!m_activeplugins.pimpl_p()->empty())
    {
        const String& id = m_activeplugins.pimpl_p()->first();
        this->unload(&m_activeplugins.pimpl_p()->value(id));
    }

    this->loadResidentPlugins(); // Reload
}

bool PluginManagerImpl::execute(const String &id, const ArgumentList& args)
{
    const PluginInstance* pi = this->find(id, REDASM_INIT_PLUGIN_NAME);
    if(!pi) return false;

    auto exec = PluginLoader::funcT<Callback_PluginExec>(pi->handle, REDASM_EXEC_NAME);
    if(!exec) return false;

    bool res = exec(args);
    this->unload(pi);
    return res;
}

void PluginManagerImpl::loadResidentPlugins()
{
    this->iteratePlugins(REDASM_INIT_PLUGIN_NAME, [&](const PluginInstance* pi) -> IterateResult {
        this->pushActive(pi);
        return IterateResult::Continue;
    });
}

void PluginManagerImpl::pushActive(const PluginInstance* pi)
{
    m_activeplugins.pimpl_p()->insert(pi->descriptor->id, *pi);

    if(pi->descriptor->plugin)
        pi->descriptor->plugin->setInstance(&m_activeplugins.pimpl_p()->value(pi->descriptor->id)); // Bind Descriptor <-> Plugin
}

bool PluginManagerImpl::iteratePlugins(const String &path, const String &initname, const PluginManagerImpl::PluginManager_Callback &cb)
{
    DIR* dir = opendir(path.c_str());

    if(!dir)
        return false;

    struct dirent* entry = nullptr;

    while((entry = readdir(dir)))
    {
        if(!std::strcmp(entry->d_name, ".") || !std::strcmp(entry->d_name, ".."))
            continue;

        if(entry->d_type == DT_DIR) // Recurse folders
        {
            String rpath = Path::create(path, entry->d_name);

            if(this->iteratePlugins(rpath.c_str(), initname, cb))
                return true;

            continue;
        }

        if(!String(entry->d_name).endsWith(SHARED_OBJECT_EXT))
            continue;

        const PluginInstance* pi = nullptr;

        if(!(pi = this->load(Path::create(path, entry->d_name), initname)))
            continue;

        IterateResult res = cb(pi);

        if(res == IterateResult::Done)
        {
            closedir(dir);
            return true;
        }

        if(res == IterateResult::Unload)
            this->unload(pi);
    }

    closedir(dir);
    return false;
}

const PluginInstance *PluginManagerImpl::find(const String &path, const String &id, const String &initname)
{
    DIR* dir = opendir(path.c_str());
    if(!dir) return nullptr;

    struct dirent* entry = nullptr;
    const PluginInstance* pi = nullptr;

    while(!pi && (entry = readdir(dir)))
    {
        if(!std::strcmp(entry->d_name, ".") || !std::strcmp(entry->d_name, ".."))
            continue;

        String rpath = Path::create(path, entry->d_name);

        if(entry->d_type == DT_DIR) // Recurse folders
        {
            pi = this->find(rpath.c_str(), id, initname);
            continue;
        }

        if(Path::fileNameOnly(entry->d_name) != id)
            continue;

        pi = this->load(rpath, initname);
    }

    closedir(dir);
    return pi;
}

const PluginInstance *PluginManagerImpl::find(const String &id, const String &initname)
{
    for(const String& pluginpath : r_ctx->pluginPaths())
    {
        const PluginInstance* pi = this->find(pluginpath.c_str(), id, initname);
        if(pi) return pi;
    }

    return nullptr;
}

} // namespace REDasm
