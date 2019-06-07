#pragma once

#define REDASM_INIT_NAME(plugintype) "redasm_init_" #plugintype

#define REDASM_LOAD_NAME            "redasm_load"
#define REDASM_UNLOAD_NAME          "redasm_unload"
#define REDASM_INIT_PLUGIN_NAME     REDASM_INIT_NAME(plugin)
#define REDASM_INIT_LOADER_NAME     REDASM_INIT_NAME(loader)
#define REDASM_INIT_ASSEMBLER_NAME  REDASM_INIT_NAME(assembler)

#define REDASM_LOAD   extern "C" bool redasm_load()
#define REDASM_UNLOAD extern "C" void redasm_unload()

#define REDASM_PLUGIN_TEMPLATE(description, author, license, version, plugintype) \
    static PluginDescriptor r_plugin = { REDASM_API_VERSION, version, description, author, license, r_plugin_id }; \
    extern "C" LIBREDASM_API REDasm::PluginDescriptor* redasm_init_##plugintype(REDasm::Context* ctx) { \
        REDasm::Context::init(ctx); \
        return &r_plugin; \
    }

#define REDASM_PLUGIN(description, author, license, version)  REDASM_PLUGIN_TEMPLATE(description, author, license, version, plugin)
#define REDASM_LOADER(description, author, license, version)  REDASM_PLUGIN_TEMPLATE(description, author, license, version, loader)
#define REDASM_ASSEMBLER(descriptionauthor, license, version) REDASM_PLUGIN_TEMPLATE(description, author, license, version, assembler)

#include "../types/base_types.h"
#include "../types/callback_types.h"
#include "../version.h"
#include "../macros.h"
#include "../pimpl.h"

namespace REDasm {

struct PluginDescriptor;
class PluginImpl;

class Plugin
{
    PIMPL_DECLARE_PRIVATE(Plugin)

    protected:
        Plugin(PluginImpl* p);

    public:
        Plugin();
        virtual ~Plugin() = default;
        std::string id() const;
        std::string description() const;
        const PluginDescriptor* descriptor() const;
        void setDescriptor(const PluginDescriptor* pd);
};

struct LIBREDASM_API PluginDescriptor
{
    version_t apiVersion, version;
    const char *description, *author, *license, *id;
    Plugin* plugin;
};

} // namespace REDasm
