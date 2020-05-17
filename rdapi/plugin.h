#pragma once

#include "macros.h"
#include "level.h"
#include "types.h"

#ifdef __cplusplus
    #define RD_PLUGIN_DECLARE(type, pluginid) static type pluginid = {};
#else
    #define RD_PLUGIN_DECLARE(type, pluginid) static type pluginid = {0};
#endif

#define RD_PLUGIN_CREATE(type, pluginid, pluginname) \
    RD_PLUGIN_DECLARE(type, pluginid)  \
    pluginid.apilevel = RDAPI_LEVEL;   \
    pluginid.apibits = sizeof(size_t); \
    pluginid.id = #pluginid;           \
    pluginid.name = pluginname;        \
    pluginid.state = PluginState_Loaded

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

typedef void (*Callback_PluginInit)(RDPluginHeader*);
typedef void (*Callback_PluginFree)(RDPluginHeader*);

enum PluginState: size_t {
    PluginState_Loaded,
    PluginState_Initialized,
};

typedef struct RDPluginHeader {
    RD_PLUGIN_HEADER
} RDPluginHeader;

RD_API_EXPORT void RDPlugin_Free(RDPluginHeader* plugin);
