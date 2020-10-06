#pragma once

#include "../macros.h"
#include "../level.h"
#include "../types.h"

#define RD_ENTRY_DECLARE(type, entryid) static type entryid;

#ifdef __cplusplus
    #define RD_ENTRY_INIT(entry, entryname) \
        entry = { }; \
        entry.apilevel = RDAPI_LEVEL;   \
        entry.apibits = sizeof(size_t); \
        entry.id = #entry;             \
        entry.name = entryname;
#else
    #define RD_ENTRY_INIT(entry, entryname) \
        entry = {0}; \
        entry.apilevel = RDAPI_LEVEL;   \
        entry.apibits = sizeof(size_t); \
        entry.id = #entry;             \
        entry.name = entryname;
#endif

#define RD_PLUGIN_ENTRY(type, entryid, entryname) \
    RD_ENTRY_DECLARE(type, entryid) \
    RD_ENTRY_INIT(entryid, entryname)

#define RD_ENTRY_HEADER \
    apilevel_t apilevel; \
    u32 apibits; \
    const char *id, *name;

struct RDContext;

DECLARE_HANDLE(RDPluginModule);

#ifdef rd_plugin_id
RD_API_EXPORT void rdplugin_init(RDContext* ctx, RDPluginModule* pm);
RD_API_EXPORT void rdplugin_free(RDContext* ctx);
#else
typedef void (*Callback_PluginInit)(RDContext* ctx, RDPluginModule* pm);
typedef void (*Callback_PluginFree)(RDContext* ctx);
#endif

typedef struct RDEntry {
    RD_ENTRY_HEADER
} RDEntry;
