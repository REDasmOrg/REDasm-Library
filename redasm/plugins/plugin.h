#pragma once

#define REDASM_INIT_NAME(plugintype) "redasm_init_" #plugintype

#define REDASM_LOAD_NAME            "redasm_load"
#define REDASM_UNLOAD_NAME          "redasm_unload"
#define REDASM_EXEC_NAME            "redasm_exec"
#define REDASM_INIT_PLUGIN_NAME     REDASM_INIT_NAME(plugin)
#define REDASM_INIT_LOADER_NAME     REDASM_INIT_NAME(loader)
#define REDASM_INIT_ASSEMBLER_NAME  REDASM_INIT_NAME(assembler)

#define REDASM_EXEC   extern "C" LIBREDASM_API bool redasm_exec(const REDasm::ArgumentList& args)
#define REDASM_LOAD   extern "C" LIBREDASM_API bool redasm_load()
#define REDASM_UNLOAD extern "C" LIBREDASM_API void redasm_unload()

#define REDASM_PLUGIN_TEMPLATE(id, description, author, license, level, plugintype) \
    static REDasm::PluginDescriptor id = { REDASM_API_LEVEL, level, description, author, license, #id, nullptr }; \
    extern "C" LIBREDASM_API REDasm::PluginDescriptor* redasm_init_##plugintype(REDasm::Context* ctx) { \
        REDasm::Context::init(ctx); \
        return &id; \
    }

#define REDASM_PLUGIN_ID(id, description, author, license, level)    REDASM_PLUGIN_TEMPLATE(id, description, author, license, level, plugin)
#define REDASM_LOADER_ID(id, description, author, license, level)    REDASM_PLUGIN_TEMPLATE(id, description, author, license, level, loader)
#define REDASM_ASSEMBLER_ID(id, description, author, license, level) REDASM_PLUGIN_TEMPLATE(id, description, author, license, level, assembler)

#define REDASM_PLUGIN(description, author, license, level)    REDASM_PLUGIN_ID(r_plugin, description, author, license, level)
#define REDASM_LOADER(description, author, license, level)    REDASM_LOADER_ID(r_plugin, description, author, license, level)
#define REDASM_ASSEMBLER(description, author, license, level) REDASM_ASSEMBLER_ID(r_plugin, description, author, license, level)

#include "../support/containers/argumentlist.h"
#include "../types/base_types.h"
#include "../types/plugin_types.h"
#include "../types/callback_types.h"
#include "../level.h"
#include "../macros.h"
#include "../pimpl.h"
#include "plugininstance.h"

namespace REDasm {

struct PluginDescriptor;
class PluginImpl;

class Plugin: public Object
{
    PIMPL_DECLARE_P(Plugin)
    PIMPL_DECLARE_PRIVATE(Plugin)

    protected:
        Plugin(PluginImpl* p);

    public:
        Plugin();
        virtual ~Plugin() = default;
        std::string id() const;
        std::string description() const;
        const PluginDescriptor* descriptor() const;
        const PluginInstance* instance() const;
        void setInstance(const PluginInstance* pi);
};

struct LIBREDASM_API PluginDescriptor
{
    apilevel_t apiLevel, level;
    const char *description, *author, *license, *id;
    Plugin* plugin;
};

template<typename T> T* plugin_cast(const PluginInstance* pi) { return static_cast<T*>(pi->descriptor->plugin); }

} // namespace REDasm
