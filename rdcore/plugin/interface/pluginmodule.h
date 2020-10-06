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
    typedef HMODULE library_t;
#else
    #include <dlfcn.h>
    typedef void* library_t;
#endif

class PluginModule: public Object
{
    public:
        typedef std::pair<size_t, const RDEntry*> EntryItem;

    public:
        PluginModule(Context* ctx);
        PluginModule(Context* ctx, const std::string& filepath);
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
        template<typename Function> Function funcT(const char* name) { return reinterpret_cast<Function>(func(name)); }
        std::string fileName() const;
        void* func(const char* name);
        void unload();

    private:
        std::vector<EntryItem> m_entries;
        library_t m_handle{ };
        std::string m_filepath;
        Callback_PluginInit m_init;
        Callback_PluginFree m_free;
};

template<typename EntryType>
bool PluginModule::registerEntry(size_t c, EntryType* e) {
    if(!e->name || !std::strlen(e->name)) {
        this->log("Invalid entry name");
        return false;
    }

    auto cit = std::find_if(m_entries.begin(), m_entries.end(), [&](const auto& item) {
        return (item.first == c) && !std::strcmp(item.second->id, e->id);
    });

    if(cit != m_entries.end()) {
        this->log("Duplicate module entry: " + Utils::quoted(e->id));
        return false;
    }

    m_entries.push_back({c, reinterpret_cast<const RDEntry*>(e)});
    return true;
}

typedef std::shared_ptr<PluginModule> PluginModulePtr;
