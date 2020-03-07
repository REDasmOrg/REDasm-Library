#pragma once

#include <unordered_map>
#include <functional>
#include <redasm/pimpl.h>
#include <redasm/plugins/pluginmanager.h>
#include "../types/containers/templates/list_template.h"
#include "../types/containers/templates/dictionary_template.h"

namespace REDasm {

class PluginListImpl: public ListTemplate<const PluginInstance*>
{
    PIMPL_DECLARE_Q(PluginList)
    PIMPL_DECLARE_PUBLIC(PluginList)

    public:
        void weightSort();
        void alphaSort();
};

class PluginMapImpl: public DictionaryTemplate< std::unordered_map<String, PluginInstance>, std::unordered_set<String> >
{
    PIMPL_DECLARE_Q(PluginMap)
    PIMPL_DECLARE_PUBLIC(PluginMap)

};

class PluginManagerImpl
{
    PIMPL_DECLARE_Q(PluginManager)
    PIMPL_DECLARE_PUBLIC(PluginManager)

    public:
        enum class IterateResult { Done = 0, Continue, Unload };
        typedef std::function<IterateResult(const PluginInstance*)> PluginManager_Callback;

    public:
        PluginManagerImpl();
        const PluginInstance* load(const String& pluginpath, const String& initname);
        const PluginInstance* find(const String& id, const String& initname);
        void scanPlugins(const String& initname, const PluginManager_Callback& cb);
        void unload(const PluginInstance* pi);
        void unloadAll();

    public:
        bool execute(const String& id, const ArgumentList& args);

    private:
        void loadResidentPlugins();
        void pushActive(const PluginInstance* pi);
        bool scanPlugins(const String& path, const String& initname, const PluginManager_Callback& cb);
        const PluginInstance* find(const String& path, const String& id, const String& initname);

    private:
        PluginMap m_activeplugins;
};

} // namespace REDasm
