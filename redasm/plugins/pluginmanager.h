#pragma once

#include <unordered_map>
#include <deque>
#include "../plugins/loader/loader.h"
#include "../types/base_types.h"
#include "../macros.h"
#include "../pimpl.h"

namespace REDasm {

class PluginManagerImpl;

class LIBREDASM_API PluginManager
{
    PIMPL_DECLARE_PRIVATE(PluginManager)

    public:
        typedef std::unordered_map<std::string, PluginInstance> PluginMap;
        typedef std::deque<const PluginInstance*> PluginList;

    private:
        PluginManager();
        static PluginManager* instance();
        const PluginInstance* find(const std::string& id, const char* initname);

    public:
        ~PluginManager();
        void unloadAll();
        void unload(const PluginInstance* pi);
        void unload(const PluginList& pl);
        const PluginMap& activePlugins() const;
        const PluginInstance* findLoader(const std::string& id);
        const PluginInstance* findAssembler(const std::string& id);
        const PluginInstance* findPlugin(const std::string& id);
        PluginList getLoaders(const REDasm::LoadRequest *request);
        PluginList getAssemblers();

    friend class Context;
};

} // namespace REDasm
