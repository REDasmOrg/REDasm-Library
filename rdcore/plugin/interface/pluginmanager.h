#pragma once

#include <rdapi/plugin/entry.h>
#include <unordered_map>
#include <string>
#include "pluginmodule.h"
#include "../../object.h"

class PluginManager: public Object
{
    private:
        typedef std::vector<const RDEntry*> EntryList;

    public:
        PluginManager(Context* ctx);
        const EntryList& loaders() const;
        const EntryList& assemblers() const;
        const EntryList& analyzers() const;
        const RDEntryAssembler* getAssembler(const std::string& id) const;
        const RDEntryAssembler* findAssembler(const std::string& id) const;
        const RDEntryAssembler* selectAssembler(const std::string& id);
        const RDEntryLoader* selectLoader(const std::string& id);
        bool executeCommand(const std::string& cmd, const RDArguments* a) const;
        void checkCommands();
        void unload(const RDEntry* entry);

    private:
        template<typename T> const T* findEntry(size_t c, const std::string& id) const;
        const RDEntry* selectEntry(size_t c, const std::string& id);
        void loadAll(const fs::path& pluginpath);
        void load(const fs::path& filepath);
        void load(const PluginModulePtr& pm);
        bool checkArguments(const RDEntryCommand* command, const RDArguments* a) const;
        void loadBuiltins();

    private:
        std::unordered_map<size_t, EntryList> m_entries;
        std::unordered_map<std::string, PluginModulePtr> m_modules;
        std::unordered_map<std::string, const RDEntryCommand*> m_commands;
};

template<typename T>
const T* PluginManager::findEntry(size_t c, const std::string& id) const {
    auto cit = m_entries.find(c);
    if(cit == m_entries.end()) return nullptr;

    auto it = std::find_if(cit->second.begin(), cit->second.end(), [&](const RDEntry* e) {
              return e->id == id;
    });

    return (it != cit->second.end()) ? reinterpret_cast<const T*>(*it) : nullptr;
}
