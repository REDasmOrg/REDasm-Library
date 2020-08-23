#pragma once

#include "macros.h"
#include "level.h"
#include "types.h"

#define RD_PLUGIN_DECLARE(type, pluginid) static type pluginid;

#ifdef __cplusplus
    #define RD_PLUGIN_INIT(plugin, pluginname) \
        plugin = { }; \
        plugin.apilevel = RDAPI_LEVEL;   \
        plugin.apibits = sizeof(size_t); \
        plugin.id = #plugin;             \
        plugin.name = pluginname;        \
        plugin.state = PluginState_Loaded
    #else
    #define RD_PLUGIN_INIT(plugin, pluginname) \
        plugin = {0}; \
        plugin.apilevel = RDAPI_LEVEL;   \
        plugin.apibits = sizeof(size_t); \
        plugin.id = #plugin;             \
        plugin.name = pluginname;        \
        plugin.state = PluginState_Loaded
#endif

#define RD_PLUGIN_CREATE(type, pluginid, pluginname) \
    RD_PLUGIN_DECLARE(type, pluginid) \
    RD_PLUGIN_INIT(pluginid, pluginname)

#define RD_PLUGIN_HEADER \
    apilevel_t apilevel; \
    u32 apibits; \
    const char *id, *name; \
    size_t state; \
    Callback_PluginInit init; \
    Callback_PluginFree free; \
    RD_USERDATA_FIELD

struct RDPluginHeader;

#ifdef rd_plugin_id
RD_API_EXPORT void redasm_entry(void);
#else
typedef void (*Callback_ModuleEntry)(void);
#endif

typedef void (*Callback_PluginInit)(RDPluginHeader* plugin);   // Plugin Initialization
typedef void (*Callback_PluginFree)(RDPluginHeader* plugin);   // Plugin Deinitialization

enum PluginState: size_t {
    PluginState_Loaded,
    PluginState_Initialized,
};

typedef struct RDPluginHeader {
    RD_PLUGIN_HEADER
} RDPluginHeader;

RD_API_EXPORT void RDPlugin_Free(RDPluginHeader* plugin);
RD_API_EXPORT bool RDPlugin_GetUserData(const RDPluginHeader* ldr, RDUserData* userdata);
