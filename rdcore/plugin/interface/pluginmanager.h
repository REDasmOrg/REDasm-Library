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
        const RDEntryAssembler* findAssembler(const std::string& id) const;
        const RDEntryLoader* selectLoader(const std::string& id);
        const RDEntryAssembler* selectAssembler(const std::string& id);
        void unload(const RDEntry* entry);

    private:
        template<typename T> const T* findEntry(size_t c, const std::string& id) const;
        const RDEntry* selectEntry(size_t c, const std::string& id);
        void loadBuiltins();
        void loadAll(const std::string& pluginpath);
        void load(const std::string& filepath);
        void load(const PluginModulePtr& pm);

    private:
        std::unordered_map<size_t, EntryList> m_entries;
        std::unordered_map<std::string, PluginModulePtr> m_modules;
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
