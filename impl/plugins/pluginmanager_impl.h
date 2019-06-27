#pragma once

#include <functional>
#include <redasm/pimpl.h>
#include <redasm/plugins/pluginmanager.h>

namespace REDasm {

class PluginManagerImpl
{
    PIMPL_DECLARE_Q(PluginManager)
    PIMPL_DECLARE_PUBLIC(PluginManager)

    public:
        enum class IterateResult { Done = 0, Continue, Unload };
        typedef std::function<IterateResult(const PluginInstance*)> PluginManager_Callback;

    public:
        PluginManagerImpl() = default;
        const PluginInstance* load(const String& pluginpath, const char *initname);
        const PluginInstance* find(const String& id, const char* initname);
        void iteratePlugins(const char* initname, const PluginManager_Callback& cb);
        void unload(const PluginInstance* pi);
        void unloadAll();

    public:
        bool execute(const PluginInstance* pi, const ArgumentList& args) const;

    private:
        bool iteratePlugins(const char* path, const char* initname, const PluginManager_Callback& cb);
        const PluginInstance* find(const char* path, const String& id, const char* initname);

    private:
        PluginManager::PluginMap m_activeplugins;
        static std::unique_ptr<PluginManager> m_instance;
};

} // namespace REDasm
