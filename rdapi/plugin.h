#pragma once

#include "macros.h"
#include "level.h"
#include "types.h"

#define RD_PLUGIN(type, pluginid, pluginname) \
    static type pluginid = []() { \
        type id##var{ }; \
        id##var.apilevel = RDAPI_LEVEL; \
        id##var.apibits = sizeof(size_t); \
        id##var.id = #pluginid; \
        id##var.name = pluginname; \
        id##var.state = PluginState_Loaded; \
        return id##var; \
    }()

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
