#include "pluginmanager.h"
#include <impl/plugins/pluginmanager_impl.h>
#include <impl/plugins/pluginloader.h>
#include <redasm/plugins/plugin.h>
#include <redasm/support/path.h>
#include <redasm/context.h>

namespace REDasm {

PluginManager::PluginManager(): m_pimpl_p(new PluginManagerImpl()) { }
PluginManager::~PluginManager() { this->unloadAll(); }
void PluginManager::unloadAll() { PIMPL_P(PluginManager); p->unloadAll(); }
void PluginManager::unload(const PluginInstance *pi) { PIMPL_P(PluginManager); p->unload(pi); }

void PluginManager::unload(const PluginList &pl)
{
    for(const PluginInstance* pi : pl)
        this->unload(pi);
}

PluginManager *PluginManager::instance()
{
    if(!PluginManagerImpl::m_instance)
        PluginManagerImpl::m_instance = std::unique_ptr<PluginManager>(new PluginManager());

    return PluginManagerImpl::m_instance.get();
}

const PluginInstance *PluginManager::find(const std::string &id, const char *initname)
{
    PIMPL_P(PluginManager);

    for(const std::string& pluginpath : r_ctx->pluginPaths())
    {
        std::string pluginfilepath = Path::create(pluginpath, id + SHARED_OBJECT_EXT);

        if(Path::exists(pluginfilepath))
            return p->load(pluginfilepath, initname);
    }

    return nullptr;
}

const PluginManager::PluginMap &PluginManager::activePlugins() const { PIMPL_P(const PluginManager); return p->m_activeplugins; }
const PluginInstance *PluginManager::findLoader(const std::string &id) { return this->find(id, REDASM_INIT_LOADER_NAME); }

const PluginInstance *PluginManager::findAssembler(const char* id)
{
    if(!id)
        return nullptr;

    return this->find(std::string(id), REDASM_INIT_ASSEMBLER_NAME);
}

const PluginInstance *PluginManager::findPlugin(const std::string &id) { return this->find(id, REDASM_INIT_PLUGIN_NAME); }

PluginManager::PluginList PluginManager::getLoaders(const LoadRequest& request)
{
    PIMPL_P(PluginManager);
    PluginList plugins;

    p->iteratePlugins(REDASM_INIT_LOADER_NAME, [&plugins, request](const PluginInstance* pi) -> PluginManagerImpl::IterateResult {
        Loader* loader = static_cast<Loader*>(pi->descriptor->plugin);
        bool res = loader->test(request);

        if(res) {
            plugins.push_back(pi);
            return PluginManagerImpl::IterateResult::Continue;
        }

        return PluginManagerImpl::IterateResult::Unload;
    });

    return plugins;
}

PluginManager::PluginList PluginManager::getAssemblers()
{
    PIMPL_P(PluginManager);
    PluginList plugins;

    p->iteratePlugins(REDASM_INIT_ASSEMBLER_NAME, [&plugins](const PluginInstance* pi) -> PluginManagerImpl::IterateResult {
        plugins.push_back(pi);
        return PluginManagerImpl::IterateResult::Continue;
    });

    return plugins;
}

bool PluginManager::execute(const PluginInstance *pi, const ArgumentList &args) const { PIMPL_P(const PluginManager); return p->execute(pi, args); }

} // namespace REDasm
