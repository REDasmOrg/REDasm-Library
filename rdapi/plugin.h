#pragma once

#include "macros.h"
#include "level.h"
#include "types.h"

#define RD_DECLARE_PLUGIN(entry) \
    RD_API_EXPORT RDPluginDescriptor redasm_plugin = { \
        RDAPI_LEVEL, \
        rd_plugin_id, \
        entry \
    };

#define RD_PLUGIN(type, id, name, init, free, ...) \
    static type id = { \
        { #id, name, PluginState_Loaded, init, free, { nullptr } }, \
        __VA_ARGS__ \
    };

struct RDPluginHeader;

typedef void (*Callback_PluginEntry)(void);
typedef void (*Callback_PluginInit)(RDPluginHeader*);
typedef void (*Callback_PluginFree)(RDPluginHeader*);

enum PluginState: size_t {
    PluginState_Loaded,
    PluginState_Initialized,
};

typedef struct RDPluginHeader {
    const char *id, *name;
    size_t state;

    Callback_PluginInit init;
    Callback_PluginFree free;

    union {
        void* puserdata;
        uintptr_t userdata;
    };

} RDPluginHeader;

typedef struct RDPluginDescriptor {
    apilevel_t apiLevel;
    const char *id;

    Callback_PluginEntry entry;
} RDPluginDescriptor;

typedef struct RDPluginInstance {
    void* handle;
    const char *path;
    const RDPluginDescriptor* descriptor;
} RDPluginInstance;

RD_API_EXPORT void RDPlugin_Free(RDPluginHeader* plugin);
