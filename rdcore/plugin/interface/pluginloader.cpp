#include "pluginloader.h"
#include "../../context.h"
#include <filesystem>
#include <cstdlib>
#include <string.h>

#define REDASM_PLUGIN_STRUCT_NAME "redasm_plugin"

bool PluginLoader::load(const std::string& pluginpath, RDPluginInstance& pi)
{
    LoadResult lr;

    if((lr = PluginLoader::loadLibrary(pluginpath, pi)) != LoadResult::Ok)
    {
        std::string pluginfilename = std::filesystem::path(pluginpath).filename();

        if(lr == LoadResult::Failed) rd_ctx->log(pluginfilename + ": Loading failed");
        else if(lr == LoadResult::InvalidEntry) rd_ctx->log(pluginfilename + ": '" + REDASM_PLUGIN_STRUCT_NAME + "': Not found");
        return false;
    }

    return true;
}

void PluginLoader::unload(const RDPluginInstance* pi)
{
    std::free(const_cast<char*>(pi->path));
    PluginLoader::unloadLibrary(pi->handle);
}

void* PluginLoader::func(library_t lib, const char* name)
{
#ifdef _WIN32
    return reinterpret_cast<void*>(GetProcAddressA(lib, name));
#else
    return reinterpret_cast<void*>(dlsym(lib, name));
#endif
}

PluginLoader::LoadResult PluginLoader::loadLibrary(const std::string& pluginpath, RDPluginInstance& pi)
{
#ifdef _WIN32
    pi.handle = LoadLibraryA(pluginpath.c_str());
#else
    pi.handle = dlopen(pluginpath.c_str(), RTLD_LAZY);
#endif

    if(!pi.handle)
    {
#ifdef _WIN32
#else
        rd_ctx->log(dlerror());
#endif
        return LoadResult::Failed;
    }

    pi.descriptor = PluginLoader::funcT<RDPluginDescriptor*>(pi.handle, REDASM_PLUGIN_STRUCT_NAME);

    if(!pi.descriptor || !pi.descriptor->entry)
    {
        PluginLoader::unloadLibrary(pi.handle);
        return LoadResult::InvalidEntry;
    }

    pi.path = strdup(pluginpath.c_str());
    pi.descriptor->entry();
    return LoadResult::Ok;
}

void PluginLoader::unloadLibrary(library_t lib)
{
#ifdef _WIN32
    FreeLibrary(lib);
#else
    dlclose(lib);
#endif
}
