#pragma once

#include "../plugins/loader/loader.h"
#include "../types/base.h"
#include "../macros.h"
#include "../pimpl.h"

namespace REDasm {

class PluginMapImpl;
class PluginListImpl;
class PluginManagerImpl;

class PluginList: public Object
{
    REDASM_OBJECT(PluginList)
    PIMPL_DECLARE_P(PluginList)
    PIMPL_DECLARE_PRIVATE(PluginList)

    public:
        PluginList();
        const PluginInstance* at(size_t idx) const;
        const PluginInstance* first() const;
        const PluginInstance* last() const;
        void erase(const PluginInstance* pi);
        bool empty() const;
        size_t size() const;

    friend class PluginManager;
};

class PluginMap: public Object
{
    REDASM_OBJECT(PluginMap)
    PIMPL_DECLARE_P(PluginMap)
    PIMPL_DECLARE_PRIVATE(PluginMap)

    public:
        PluginMap();

    friend class PluginManagerImpl;
};

class LIBREDASM_API PluginManager: public Object
{
    REDASM_OBJECT(PluginManager)
    PIMPL_DECLARE_P(PluginManager)
    PIMPL_DECLARE_PRIVATE(PluginManager)

    private:
        PluginManager();
        static PluginManager* instance();

    public:
        ~PluginManager();
        void shutdown();
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
