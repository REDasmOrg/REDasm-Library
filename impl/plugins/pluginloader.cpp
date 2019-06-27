#include "pluginloader.h"
#include <redasm/support/utils.h>
#include <redasm/support/path.h>
#include <redasm/context.h>

namespace REDasm {

bool PluginLoader::load(const String &pluginpath, const char* initname, PluginInstance* pi)
{
    LoadResult lr;

    if((lr = PluginLoader::loadLibrary(pluginpath, initname, pi)) != LoadResult::Ok)
    {
        if(lr == LoadResult::Failed)
            r_ctx->log(Path::fileName(pluginpath) + ": Loading failed");

        return false;
    }

    auto f = PluginLoader::funcT<Callback_ReturnBool>(pi->handle, REDASM_LOAD_NAME);

    if(!f)
        return true;

    if(!f() || !pi->descriptor->plugin)
    {
        if(!pi->descriptor->plugin)
            r_ctx->log(Path::fileName(pluginpath) + ": Plugin field is NULL");
        else
            r_ctx->log(Path::fileName(pluginpath) + ": Call to " + String(REDASM_LOAD_NAME).quoted() + " failed");

        PluginLoader::unloadLibrary(pi->handle);
        return false;
    }

    return true;
}

void PluginLoader::unload(const PluginInstance* pi)
{
    auto f = PluginLoader::funcT<Callback_ReturnVoid>(pi->handle, REDASM_UNLOAD_NAME);

    if(f)
        f();

    PluginLoader::unloadLibrary(pi->handle);
}

void* PluginLoader::func(library_t lib, const char *fn)
{
#ifdef _WIN32
    return reinterpret_cast<void*>(GetProcAddressA(lib, fn));
#else
    return reinterpret_cast<void*>(dlsym(lib, fn));
#endif
}

void PluginLoader::unloadLibrary(library_t lib)
{
#ifdef _WIN32
    FreeLibrary(lib);
#else
    dlclose(lib);
#endif
}

PluginLoader::LoadResult PluginLoader::loadLibrary(const String &pluginpath, const char* initname, PluginInstance* pi)
{
#ifdef _WIN32
    pi->handle = LoadLibraryA(pluginpath.c_str());
#else
    pi->handle = dlopen(pluginpath.c_str(), RTLD_LAZY);
#endif

    if(!pi->handle)
        return LoadResult::Failed;

    auto f = PluginLoader::funcT<Callback_PluginInit>(pi->handle, initname);

    if(!f)
    {
        PluginLoader::unloadLibrary(pi->handle);
        return LoadResult::InvalidInit;
    }

    pi->descriptor = f(r_ctx);

    if(!pi->descriptor)
    {
        PluginLoader::unloadLibrary(pi->handle);
        return LoadResult::Failed;
    }

    pi->path = pluginpath;
    return LoadResult::Ok;
}

} // namespace REDasm
