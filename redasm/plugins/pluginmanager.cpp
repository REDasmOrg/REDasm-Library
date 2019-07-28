#include "pluginmanager.h"
#include <impl/plugins/pluginmanager_impl.h>
#include <impl/plugins/pluginloader.h>
#include <redasm/plugins/assembler/assembler.h>
#include <redasm/plugins/loader/loader.h>
#include <redasm/plugins/plugin.h>
#include <redasm/support/path.h>
#include <redasm/context.h>

namespace REDasm {

PluginList::PluginList(): m_pimpl_p(new PluginListImpl()) { }

const PluginInstance *PluginList::at(size_t idx) const { PIMPL_P(const PluginList); return p->at(idx); }
const PluginInstance *PluginList::first() const { PIMPL_P(const PluginList); return p->first(); }
const PluginInstance *PluginList::last() const { PIMPL_P(const PluginList); return p->last(); }
void PluginList::erase(const PluginInstance *pi) { PIMPL_P(PluginList); p->erase(pi); }
bool PluginList::empty() const { PIMPL_P(const PluginList); return p->empty(); }
size_t PluginList::size() const { PIMPL_P(const PluginList); return p->size(); }

PluginManager::PluginManager(): m_pimpl_p(new PluginManagerImpl()) { }
PluginManager::~PluginManager() { this->unloadAll(); }
void PluginManager::unloadAll() { PIMPL_P(PluginManager); p->unloadAll(); }
void PluginManager::unload(const PluginInstance *pi) { PIMPL_P(PluginManager); p->unload(pi); }

void PluginManager::unload(const PluginList &pl)
{
    for(size_t i = 0; i < pl.size(); i++)
        this->unload(pl.at(i));
}

PluginManager *PluginManager::instance()
{
    static PluginManager instance;
    return &instance;
}

const PluginMap &PluginManager::activePlugins() const { PIMPL_P(const PluginManager); return p->m_activeplugins; }
const PluginInstance *PluginManager::findLoader(const String &id) { PIMPL_P(PluginManager); return p->find(id, REDASM_INIT_LOADER_NAME); }
const PluginInstance *PluginManager::findAssembler(const String &id) { PIMPL_P(PluginManager); return p->find(id, REDASM_INIT_ASSEMBLER_NAME); }
const PluginInstance *PluginManager::findPlugin(const String &id) { PIMPL_P(PluginManager); return p->find(id, REDASM_INIT_PLUGIN_NAME); }

PluginList PluginManager::getLoaders(const LoadRequest& request)
{
    PIMPL_P(PluginManager);
    PluginList plugins;

    p->iteratePlugins(REDASM_INIT_LOADER_NAME, [&plugins, request](const PluginInstance* pi) -> PluginManagerImpl::IterateResult {
        Loader* loader = plugin_cast<Loader>(pi);
        bool res = loader->test(request);

        if(res) {
            loader->init(request);
            plugins.pimpl_p()->append(pi);
            return PluginManagerImpl::IterateResult::Continue;
        }

        return PluginManagerImpl::IterateResult::Unload;
    });

    plugins.pimpl_p()->weightSort();
    return plugins;
}

PluginList PluginManager::getAssemblers()
{
    PIMPL_P(PluginManager);
    PluginList plugins;

    p->iteratePlugins(REDASM_INIT_ASSEMBLER_NAME, [&plugins](const PluginInstance* pi) -> PluginManagerImpl::IterateResult {
        plugins.pimpl_p()->append(pi);
        return PluginManagerImpl::IterateResult::Continue;
    });

    plugins.pimpl_p()->alphaSort();
    return plugins;
}

bool PluginManager::execute(const String &id, const ArgumentList &args) { PIMPL_P(PluginManager); return p->execute(id, args); }
bool PluginManager::execute(const String &id) { return this->execute(id, { }); }

PluginMap::PluginMap(): m_pimpl_p(new PluginMapImpl()) { }

} // namespace REDasm
