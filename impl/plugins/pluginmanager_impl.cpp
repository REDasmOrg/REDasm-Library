#include "pluginmanager_impl.h"
#include "pluginloader.h"
#include <redasm/support/filesystem.h>
#include <redasm/context.h>

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

PluginManagerImpl::PluginManagerImpl() { this->loadResidentPlugins(); }

const PluginInstance *PluginManagerImpl::load(const String &pluginpath, const String &initname)
{
    PluginInstance pi;
    if(!PluginLoader::load(pluginpath, initname, &pi)) return nullptr;

    this->pushActive(&pi);
    return &m_activeplugins.pimpl_p()->value(pi.descriptor->id);
}

void PluginManagerImpl::scanPlugins(const String &initname, const PluginManager_Callback &cb)
{
    REDasm::list_adapter_ptr<String> adapter(r_ctx->pluginPaths());

    for(size_t i = 0; i < adapter->size(); i++)
        this->scanPlugins(adapter->at(i).c_str(), initname, cb);
}

void PluginManagerImpl::unloadAll()
{
    this->shutdown();
    this->loadResidentPlugins(); // Reload
}

void PluginManagerImpl::shutdown()
{
    while(!m_activeplugins.pimpl_p()->empty())
    {
        const String& id = m_activeplugins.pimpl_p()->first();
        this->unload(&m_activeplugins.pimpl_p()->value(id));
    }
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
    this->scanPlugins(REDASM_INIT_PLUGIN_NAME, [&](const PluginInstance* pi) -> IterateResult {
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

void PluginManagerImpl::scanPlugins(const String &path, const String &initname, const PluginManagerImpl::PluginManager_Callback &cb)
{
    REDasm::list_adapter_ptr<FS::Entry> entries(FS::recurse(path));

    for(size_t i = 0; i < entries->size(); i++)
    {
        const FS::Entry& entry = entries->at(i);
        if(entry.path.ext() != SHARED_OBJECT_EXT) continue;

        const PluginInstance* pi = nullptr;
        if(!(pi = this->load(entry.value(), initname))) continue;

        IterateResult res = cb(pi);
        if(res == IterateResult::Done) return;
        if(res == IterateResult::Unload) this->unload(pi);
    }
}

const PluginInstance *PluginManagerImpl::find(const String &path, const String &id, const String &initname)
{
    REDasm::list_adapter_ptr<FS::Entry> entries(FS::recurse(path));
    const PluginInstance* pi = nullptr;

    for(size_t i = 0; i < entries->size(); i++)
    {
        const FS::Entry& entry = entries->at(i);
        if(entry.path.ext() != SHARED_OBJECT_EXT) continue;
        if(entry.path.stem() != id) continue;

        pi = this->load(entry.value(), initname);
        break;
    }

    return pi;
}

const PluginInstance *PluginManagerImpl::find(const String &id, const String &initname)
{
    REDasm::list_adapter_ptr<String> adapter(r_ctx->pluginPaths());

    for(size_t i = 0; i < adapter->size(); i++)
    {
        const PluginInstance* pi = this->find(adapter->at(i).c_str(), id, initname);
        if(pi) return pi;
    }

    return nullptr;
}

} // namespace REDasm
