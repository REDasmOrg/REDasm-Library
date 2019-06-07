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
        static bool load(const std::string& pluginpath, const char *initname, PluginInstance *pi);
        static void unload(const PluginInstance *pi);

    private:
        template<typename Function> static Function funcT(library_t lib, const char* fn) { return reinterpret_cast<Function>(func(lib, fn)); }
        static void* func(library_t lib, const char* fn);
        static LoadResult loadLibrary(const std::string& pluginpath, const char *initname, PluginInstance *pi);
        static void unloadLibrary(library_t lib);

};

} // namespace REDasm
