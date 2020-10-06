#include "pluginmanager.h"
#include "../../builtin/builtin.h"
#include "category.h"
#include <filesystem>

PluginManager::PluginManager(Context* ctx): Object(ctx)
{
    for(size_t i = 0; i < EntryCategory_Last; i++) m_entries[i] = { }; // Initialize all categories
    for(const std::string& pluginpath : rd_cfg->pluginPaths()) this->loadAll(pluginpath);
    this->loadBuiltins();
}

const PluginManager::EntryList& PluginManager::loaders() const { return m_entries.at(EntryCategory_Loader); }
const PluginManager::EntryList& PluginManager::assemblers() const { return m_entries.at(EntryCategory_Assembler); }
const PluginManager::EntryList& PluginManager::analyzers() const { return m_entries.at(EntryCategory_Analyzer); }
const RDEntryAssembler* PluginManager::findAssembler(const std::string& id) const { return this->findEntry<RDEntryAssembler>(EntryCategory_Assembler, id); }
const RDEntryLoader* PluginManager::selectLoader(const std::string& id) { return reinterpret_cast<const RDEntryLoader*>(this->selectEntry(EntryCategory_Loader, id)); }
const RDEntryAssembler* PluginManager::selectAssembler(const std::string& id) { return reinterpret_cast<const RDEntryAssembler*>(this->selectEntry(EntryCategory_Assembler, id)); }

void PluginManager::loadAll(const std::string& pluginpath)
{
    std::filesystem::directory_entry e(pluginpath);
    if(!e.is_directory()) return;

    for(const auto& entry : std::filesystem::recursive_directory_iterator(e, std::filesystem::directory_options::follow_directory_symlink))
    {
        if(entry.is_directory() || entry.path().extension() != SHARED_OBJECT_EXT) continue;
        this->load(entry.path());
    }
}

void PluginManager::unload(const RDEntry* entry) { m_modules.erase(entry->id); }

const RDEntry* PluginManager::selectEntry(size_t c, const std::string& id)
{
    auto& e = m_entries[c];
    const RDEntry* entry = nullptr;

    for(auto it = e.begin(); it != e.end(); )
    {
        if((*it)->id != id)
        {
            m_modules.erase((*it)->id);
            it = e.erase(it);
        }
        else
        {
            entry = *it;
            it++;
        }
    }

    if(!entry) this->log("Cannot select " + Utils::quoted(id));
    return entry;
}

void PluginManager::loadBuiltins()
{
    auto pm = std::make_shared<PluginModule>(this->context());
    for(const auto& [category, entry] : BUILTINS.entries) pm->registerEntry(category, entry);
    this->load(pm);
}

void PluginManager::load(const std::string& filepath)
{
    auto pm = std::make_shared<PluginModule>(this->context(), filepath);
    if(pm->loaded()) this->load(pm);
}

void PluginManager::load(const PluginModulePtr& pm)
{
    for(const auto& [category, entry] : pm->entries())
    {
        if(m_modules.count(entry->id))
        {
            this->log("Duplicate entry: " + Utils::quoted(entry->id));
            continue;
        }

        m_entries[category].push_back(entry);
        m_modules[entry->id] = pm;
    }
}
