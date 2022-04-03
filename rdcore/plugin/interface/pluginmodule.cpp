#include "pluginmodule.h"
#include "../../support/utils.h"
#include "../../config.h"
#include "category.h"
#include <rdapi/context.h>
#include <unordered_set>
#include <filesystem>

#define RDPLUGIN_PLUGIN_INIT_NAME "rdplugin_init"
#define RDPLUGIN_PLUGIN_FREE_NAME "rdplugin_free"

PluginModule::ModuleHandles PluginModule::m_sharedhandles;

PluginModule::PluginModule(Context* ctx): Object(ctx) { }

PluginModule::PluginModule(Context* ctx, const fs::path &filepath): Object(ctx), m_filepath(filepath)
{
    spdlog::debug("PluginModule::PluginModule({:p}, '{}'): Loading", reinterpret_cast<void*>(ctx), m_filepath.string());

#ifdef _WIN32
    m_handle = LoadLibraryW(m_filepath.c_str());
#else
    m_handle = dlopen(m_filepath.c_str(), RTLD_LAZY);
#endif

    if(!m_handle)
    {
#ifdef _WIN32
        //TODO: Implement Win32 Error Message Handling
#else
        spdlog::error("PluginModule::PluginModule({:p}, '{}'): ", reinterpret_cast<void*>(ctx), m_filepath.string(), dlerror());
        this->log(dlerror());
#endif

        return;
    }

    m_sharedhandles[m_handle]++; // Increase shared reference count
    m_init = this->getFuncT<Callback_PluginInit>(RDPLUGIN_PLUGIN_INIT_NAME);
    m_free = this->getFuncT<Callback_PluginFree>(RDPLUGIN_PLUGIN_FREE_NAME);

    if(!m_init)
    {
        spdlog::error("PluginModule::PluginModule({:p}, '{}'): '{}' not found", reinterpret_cast<void*>(ctx), m_filepath.string(), RDPLUGIN_PLUGIN_INIT_NAME);
        this->log(this->fileName() + ": " + Utils::quoted(RDPLUGIN_PLUGIN_INIT_NAME) + ": Not found");
        this->unload();
        return;
    }

    m_init(CPTR(RDContext, this->context()), CPTR(RDPluginModule, this));
}

PluginModule::~PluginModule() { this->unload(); }
const std::vector<PluginModule::EntryItem>& PluginModule::entries() const { return m_entries; }
bool PluginModule::loaded() const { return m_handle; }
bool PluginModule::builtin() const { return m_filepath.empty() && !m_handle; }

bool PluginModule::registerEntry(const RDEntryLoader* entry) { return this->registerEntry(EntryCategory_Loader, entry); }
bool PluginModule::registerEntry(const RDEntryAssembler* entry) { return this->registerEntry(EntryCategory_Assembler, entry); }

bool PluginModule::registerEntry(const RDEntryAnalyzer* entry)
{
    if(!entry->isenabled || !entry->execute)
    {
        this->log("Invalid analyzer: " + Utils::quoted(entry->id));
        return false;
    }

    return this->registerEntry(EntryCategory_Analyzer, entry);
}

bool PluginModule::registerEntry(const RDEntryCommand* entry)
{
    if(!entry->isenabled || !entry->execute)
    {
        this->log("Invalid command: " + Utils::quoted(entry->id));
        return false;
    }

    if(!this->validateSignature(entry)) return false;
    return this->registerEntry(EntryCategory_Command, entry);
}

bool PluginModule::validateSignature(const RDEntryCommand* entry) const
{
    static const std::unordered_set<char> VALID_TYPES = { 'i', 'u', 's', 'p' };
    if(!entry->signature) return true;
    size_t len = std::strlen(entry->signature);

    if(len >= RD_ARGUMENTS_SIZE)
    {
        this->log("Too many arguments for " + Utils::quoted(entry->id) +
                  ": expected less than " + std::to_string(RD_ARGUMENTS_SIZE) + ", got " + std::to_string(len));

        return false;
    }

    for(size_t i = 0; i < len; i++)
    {
        if(VALID_TYPES.count(entry->signature[i])) continue;

        this->log("Argument " + std::to_string(i + 1) + " is not valid");
        return false;
    }

    return true;
}

std::string PluginModule::fileName() const { return m_filepath.filename().string(); }

void PluginModule::unload()
{
    spdlog::debug("PluginModule::unload(): Unloading '{}' ", m_filepath.string());

    if(!m_handle) return;
    if(m_free) m_free(CPTR(RDContext, this->context()));

    if(!(--m_sharedhandles[m_handle])) // Decrease shared reference count, keep library loaded if needed
    {
#ifdef _WIN32
        FreeLibrary(m_handle);
#else
        dlclose(m_handle);
#endif
    }

    m_handle = { };
}

void* PluginModule::getFunc(const char* name)
{
#ifdef _WIN32
    return reinterpret_cast<void*>(GetProcAddress(m_handle, name));
#else
    return reinterpret_cast<void*>(dlsym(m_handle, name));
#endif
}
