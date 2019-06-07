#pragma once

#include <unordered_map>
#include <deque>
#include "plugininstance.h"
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
        typedef std::unordered_map<std::string, PluginInstance> Plugins;
        typedef std::deque<const PluginInstance*> PluginsList;

    private:
        PluginManager();
        static PluginManager* instance();
        const PluginInstance* find(const std::string& id, const char* initname);

    public:
        ~PluginManager();
        void unload(const PluginInstance* pi);
        const Plugins& activePlugins() const;
        const PluginInstance* findLoader(const std::string& id);
        const PluginInstance* findAssembler(const std::string& id);
        const PluginInstance* findPlugin(const std::string& id);
        PluginsList getLoaders(const REDasm::LoadRequest *request);

    private:
        static std::unique_ptr<PluginManager> m_instance;

    friend class Context;
};

} // namespace REDasm
