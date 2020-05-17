#include "pluginloader.h"
#include "../../context.h"
#include <filesystem>
#include <cstdlib>
#include <string.h>

#define REDASM_MODULE_ENTRY_NAME "redasm_entry"

bool PluginLoader::load(const std::string& pluginpath, PluginInstance& pi)
{
    LoadResult lr;

    if((lr = PluginLoader::loadLibrary(pluginpath, pi)) != LoadResult::Ok)
    {
        std::string pluginfilename = std::filesystem::path(pluginpath).filename();

        if(lr == LoadResult::Failed) rd_ctx->log(pluginfilename + ": Loading failed");
        else if(lr == LoadResult::InvalidEntry) rd_ctx->log(pluginfilename + ": '" + REDASM_MODULE_ENTRY_NAME + "': Not found");
        return false;
    }

    return true;
}

void PluginLoader::unload(const PluginInstance* pi) { PluginLoader::unloadLibrary(pi->handle); }

void* PluginLoader::func(library_t lib, const char* name)
{
#ifdef _WIN32
    return reinterpret_cast<void*>(GetProcAddressA(lib, name));
#else
    return reinterpret_cast<void*>(dlsym(lib, name));
#endif
}

PluginLoader::LoadResult PluginLoader::loadLibrary(const std::string& pluginpath, PluginInstance& pi)
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

    pi.entry = PluginLoader::funcT<Callback_ModuleEntry>(pi.handle, REDASM_MODULE_ENTRY_NAME);

    if(!pi.entry)
    {
        PluginLoader::unloadLibrary(pi.handle);
        return LoadResult::InvalidEntry;
    }

    pi.path = strdup(pluginpath.c_str());
    pi.entry();
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
