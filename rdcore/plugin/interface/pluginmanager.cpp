#include "pluginmanager.h"
#include "../../builtin/builtin.h"
#include "category.h"
#include <unordered_set>
#include <filesystem>
#include <cstring>

PluginManager::PluginManager(Context* ctx): Object(ctx)
{
    for(size_t i = 0; i < EntryCategory_Last; i++) m_entries[i] = { }; // Initialize all categories
    for(const auto& pluginpath : rd_cfg->pluginPaths()) this->loadAll(pluginpath);
    this->loadBuiltins();
}

const PluginManager::EntryList& PluginManager::loaders() const { return m_entries.at(EntryCategory_Loader); }
const PluginManager::EntryList& PluginManager::assemblers() const { return m_entries.at(EntryCategory_Assembler); }
const PluginManager::EntryList& PluginManager::analyzers() const { return m_entries.at(EntryCategory_Analyzer); }
const RDEntryAssembler* PluginManager::findAssembler(const std::string& id) const { return this->findEntry<RDEntryAssembler>(EntryCategory_Assembler, id); }
const RDEntryLoader* PluginManager::selectLoader(const std::string& id) { return reinterpret_cast<const RDEntryLoader*>(this->selectEntry(EntryCategory_Loader, id)); }
const RDEntryAssembler* PluginManager::selectAssembler(const std::string& id) { return reinterpret_cast<const RDEntryAssembler*>(this->selectEntry(EntryCategory_Assembler, id)); }

bool PluginManager::executeCommand(const std::string& cmd, const RDArguments* a) const
{
    auto it = m_commands.find(cmd);

    if(it == m_commands.end())
    {
        this->log("Cannot find command " + Utils::quoted(cmd));
        return false;
    }

    if(!this->checkArguments(it->second, a)) return false;
    return it->second->execute(CPTR(RDContext, this->context()), a);
}

void PluginManager::checkCommands()
{
    auto& e = m_entries[EntryCategory_Command];

    for(auto it = e.begin(); it != e.end(); )
    {
        const auto* entrycmd = reinterpret_cast<const RDEntryCommand*>(*it);

        if(!entrycmd->isenabled || !entrycmd->isenabled(CPTR(const RDContext, this->context())))
        {
            m_modules.erase(entrycmd->id);
            it = e.erase(it);
        }
        else
        {
            m_commands[entrycmd->id] = entrycmd;
            it++;
        }
    }
}

void PluginManager::loadAll(const fs::path& pluginpath)
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

void PluginManager::load(const fs::path& filepath)
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

bool PluginManager::checkArguments(const RDEntryCommand* command, const RDArguments* a) const
{
    static const std::unordered_map<rd_type, std::pair<char, const char*>> ARGID = {
        { ArgumentType_Int,     { 'i', "int"} },
        { ArgumentType_UInt,    { 'u', "uint" } },
        { ArgumentType_String,  { 's', "string" } },
        { ArgumentType_Pointer, { 'p', "pointer" } }
    };

    size_t argc = command->signature ? std::strlen(command->signature) : 0;

    if(argc && !a)
    {
        this->log("Command " + Utils::quoted(command->id) + ": Invalid arguments");
        return false;
    }
    else if(!argc && !a) return true;

    if(argc != a->count)
    {
        this->log("Command " + Utils::quoted(command->id) +
                  ": expected " + std::to_string(argc) + " arguments, got " + std::to_string(a->count));

        return false;
    }

    for(u32 i = 0; i < a->count; i++)
    {
        auto it = ARGID.find(a->args[i].type);

        if(it == ARGID.end())
        {
            this->log("Argument " + std::to_string(i + 1) + ": invalid type");
            return false;
        }

        char s = command->signature[i];

        if(it->second.first != s)
        {
            this->log("Argument " + std::to_string(i + 1)
                      + ": expected " + Utils::quoted(it->second.second) + ", got " + std::string(ARGID.at(s).second));
            return false;
        }
    }

    return true;
}
