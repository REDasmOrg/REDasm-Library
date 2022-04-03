#pragma once

#include <rdapi/plugin/loader.h>
#include <rdapi/plugin/assembler/assembler.h>
#include <rdapi/plugin/analyzer.h>
#include <rdapi/plugin/command.h>
#include <rdapi/plugin/entry.h>
#include <cstring>
#include "../../support/utils.h"
#include "../../config.h"
#include "../../object.h"

#ifdef _WIN32
    #include <windows.h>
    #include <winbase.h>
    typedef HMODULE hmodule;
#else
    #include <dlfcn.h>
    typedef void* hmodule;
#endif

class PluginModule: public Object
{
    public:
        typedef std::pair<size_t, const RDEntry*> EntryItem;
        typedef std::unordered_map<hmodule, int> ModuleHandles;

    public:
        PluginModule(Context* ctx);
        PluginModule(Context* ctx, const fs::path& filepath);
        virtual ~PluginModule();
        const std::vector<EntryItem>& entries() const;
        bool loaded() const;
        bool builtin() const;

    public:
        bool registerEntry(const RDEntryLoader* entry);
        bool registerEntry(const RDEntryAssembler* entry);
        bool registerEntry(const RDEntryAnalyzer* entry);
        bool registerEntry(const RDEntryCommand* entry);
        template<typename EntryType> bool registerEntry(size_t c, EntryType* p);

    private:
        template<typename Function> Function getFuncT(const char* name) { return reinterpret_cast<Function>(getFunc(name)); }
        bool validateSignature(const RDEntryCommand* entry) const;
        std::string fileName() const;
        void* getFunc(const char* name);
        void unload();

    private:
        static ModuleHandles m_sharedhandles;
        std::vector<EntryItem> m_entries;
        hmodule m_handle{ };
        fs::path m_filepath;
        Callback_PluginInit m_init;
        Callback_PluginFree m_free;
};

template<typename EntryType>
bool PluginModule::registerEntry(size_t c, EntryType* e) {
    if(!e->name || !std::strlen(e->name)) {
        spdlog::warn("PluginModule::registerEntry({}, {:p}): Invalid entry name", c, reinterpret_cast<const void*>(e));
        this->log("Invalid entry name");
        return false;
    }

    auto cit = std::find_if(m_entries.begin(), m_entries.end(), [&](const auto& item) {
        return (item.first == c) && !std::strcmp(item.second->id, e->id);
    });

    if(cit != m_entries.end()) {
        spdlog::warn("PluginModule::registerEntry({}, {:p}): Duplicate module entry: '{}'", c, reinterpret_cast<const void*>(e), e->id);
        this->log("Duplicate module entry: " + Utils::quoted(e->id));
        return false;
    }

    auto* entry = reinterpret_cast<const RDEntry*>(e);
    spdlog::debug("PluginModule::registerEntry({}, {:p}): '{}' as '{}'", c, reinterpret_cast<const void*>(e), entry->name, entry->id);
    m_entries.push_back({c, entry});
    return true;
}

typedef std::shared_ptr<PluginModule> PluginModulePtr;
