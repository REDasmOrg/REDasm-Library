#pragma once

#include <unordered_map>
#include <deque>
#include "../plugins/loader/loader.h"
#include "../types/base.h"
#include "../macros.h"
#include "../pimpl.h"

namespace REDasm {

class PluginManagerImpl;

class LIBREDASM_API PluginManager
{
    PIMPL_DECLARE_P(PluginManager)
    PIMPL_DECLARE_PRIVATE(PluginManager)

    public:
        typedef std::unordered_map<String, PluginInstance> PluginMap;
        typedef std::deque<const PluginInstance*> PluginList;

    private:
        PluginManager();
        static PluginManager* instance();

    public:
        ~PluginManager();
        void unloadAll();
        void unload(const PluginInstance* pi);
        void unload(const PluginList& pl);
        const PluginMap& activePlugins() const;
        const PluginInstance* findLoader(const String& id);
        const PluginInstance* findAssembler(const String& id);
        const PluginInstance* findPlugin(const String& id);
        PluginList getLoaders(const REDasm::LoadRequest& request);
        PluginList getAssemblers();

    public:
        bool execute(const String& id, const ArgumentList& args);
        bool execute(const String& id);

    friend class Context;
};

} // namespace REDasm
