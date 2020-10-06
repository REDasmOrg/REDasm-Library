#pragma once

#include <unordered_map>
#include <rdapi/plugin/entry.h>

#define RD_BUILTIN_ENTRY(plugin, pluginname, ...) \
    { \
        RDAPI_LEVEL,        \
        sizeof(size_t),     \
        #plugin,            \
        pluginname,         \
        __VA_ARGS__         \
    }

struct Builtins
{
    Builtins();

    std::unordered_multimap<size_t, const RDEntry*> entries;
};

extern const Builtins BUILTINS;
