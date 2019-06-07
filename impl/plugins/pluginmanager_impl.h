#pragma once

#include <functional>
#include <redasm/pimpl.h>
#include <redasm/plugins/pluginmanager.h>

namespace REDasm {

class PluginManagerImpl
{
    PIMPL_DECLARE_PUBLIC(PluginManager)

    public:
        enum class IterateResult { Done = 0, Continue, Unload };
        typedef std::function<IterateResult(const PluginInstance*)> PluginManager_Callback;

    public:
        PluginManagerImpl() = default;
        void unload(const PluginInstance* pi);
        const PluginInstance* load(const std::string& pluginpath, const char *initname);
        void iteratePlugins(const char* initname, const PluginManager_Callback& cb);
        void unloadPlugins();

    private:
        PluginManager::Plugins m_activeplugins;
};

} // namespace REDasm
