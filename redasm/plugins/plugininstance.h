#pragma once

#include <string>

namespace REDasm {

#ifdef _WIN32
    #include <windows.h>
    #include <winbase.h>
    typedef HMODULE library_t;
#else
    #include <dlfcn.h>
    typedef void* library_t;
#endif

struct PluginDescriptor;

struct PluginInstance
{
    library_t handle;
    std::string path;
    PluginDescriptor* descriptor;
};

} // namespace REDasm
