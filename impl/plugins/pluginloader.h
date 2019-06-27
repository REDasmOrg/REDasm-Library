#pragma once

#include <redasm/plugins/plugininstance.h>
#include <redasm/plugins/plugin.h>

namespace REDasm {

class PluginLoader
{
    private:
        enum class LoadResult { Failed = 0, Ok, InvalidInit };

    public:
        PluginLoader() = delete;
        template<typename Function> static Function funcT(library_t lib, const char* fn) { return reinterpret_cast<Function>(func(lib, fn)); }
        static bool load(const String& pluginpath, const char *initname, PluginInstance *pi);
        static void unload(const PluginInstance *pi);

    private:
        static void* func(library_t lib, const char* fn);
        static LoadResult loadLibrary(const String& pluginpath, const char *initname, PluginInstance *pi);
        static void unloadLibrary(library_t lib);
};

} // namespace REDasm
