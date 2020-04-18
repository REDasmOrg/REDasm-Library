#pragma once

#include <rdapi/plugin.h>
#include <string>

#ifdef _WIN32
    #include <windows.h>
    #include <winbase.h>
    typedef HMODULE library_t;
#else
    #include <dlfcn.h>
    typedef void* library_t;
#endif

class PluginLoader
{
    private:
        enum LoadResult { Failed = 0, Ok, InvalidEntry };

    public:
        PluginLoader() = delete;
        static bool load(const std::string& pluginpath, RDPluginInstance& pi);
        static void unload(const RDPluginInstance* pi);

    public:
        template<typename Function> static Function funcT(library_t lib, const char* name) { return reinterpret_cast<Function>(func(lib, name)); }

    private:
        static void* func(library_t lib, const char* name);
        static LoadResult loadLibrary(const std::string& pluginpath, RDPluginInstance& pi);
        static void unloadLibrary(library_t lib);
};

