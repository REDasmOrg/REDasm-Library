#pragma once

#define RD_BUILTIN_PLUGIN(plugin, pluginname, ...) \
    { \
        RDAPI_LEVEL,        \
        sizeof(size_t),     \
        #plugin,            \
        pluginname,         \
        PluginState_Loaded, \
        nullptr,            \
        nullptr,            \
        {nullptr},          \
        __VA_ARGS__         \
    }
