#include "context.h"
#include "support/utils.h"
#include <filesystem>
#include <iostream>
#include <sstream>
#include <cstring>

#define CONTEXT_DEBOUNCE_TIMEOUT_MS 100

#define CONTEXT_DEBOUNCE_CHECK \
    auto now = std::chrono::steady_clock::now(); \
    if((now - m_laststatusreport) < m_debouncetimeout) return; \
    m_laststatusreport = now;

Context::Context(): m_rntpath(std::filesystem::current_path()), m_tmppath(std::filesystem::temp_directory_path()) { }
void Context::addPluginPath(const char* pluginpath) { m_pluginpaths.insert(pluginpath); }
void Context::addDatabasePath(const char* dbpath) { m_dbpaths.insert(dbpath);  }

void Context::setLogCallback(RD_LogCallback callback, void* userdata)
{
    m_logcallback.callback = callback;
    m_logcallback.userdata = userdata;
}

void Context::setStatusCallback(RD_StatusCallback callback, void* userdata)
{
    m_statuscallback.callback = callback;
    m_statuscallback.userdata = userdata;
}

void Context::setProgressCallback(RD_ProgressCallback callback, void* userdata)
{
    m_progresscallback.callback = callback;
    m_progresscallback.userdata = userdata;
}

void Context::statusProgress(const char* s, size_t progress)
{
    log_lock lock(m_mutex);

    if(progress) {
        CONTEXT_DEBOUNCE_CHECK
    }

    if(m_statuscallback.callback)
        m_statuscallback.callback(s, m_statuscallback.userdata);
    else
        std::cout << s << std::endl;

    if(m_progresscallback.callback)
        m_progresscallback.callback(progress, m_progresscallback.userdata);
}

void Context::statusAddress(const char* s, address_t address)
{
    log_lock lock(m_mutex);
    CONTEXT_DEBOUNCE_CHECK

    std::stringstream ss;
    ss << s << " @ " << Utils::hex(address);

    if(m_statuscallback.callback)
        m_statuscallback.callback(ss.str().c_str(), m_statuscallback.userdata);
    else
        std::cout << ss.str() << std::endl;
}

void Context::status(const char* s)
{
    log_lock lock(m_mutex);
    CONTEXT_DEBOUNCE_CHECK

    if(m_statuscallback.callback)
        m_statuscallback.callback(s, m_statuscallback.userdata);
    else
        std::cout << s << std::endl;
}

void Context::log(const char* s)
{
    log_lock lock(m_mutex);
    CONTEXT_DEBOUNCE_CHECK

    if(m_logcallback.callback)
        m_logcallback.callback(s, m_logcallback.userdata);
    else
        std::cout << s << std::endl;
}

void Context::sync(bool b) { m_sync = b; }
bool Context::registerPlugin(RDLoaderPlugin* ploader) { return this->registerPlugin(reinterpret_cast<RDPluginHeader*>(ploader), m_loaders); }
bool Context::registerPlugin(RDAssemblerPlugin* passembler) { return this->registerPlugin(reinterpret_cast<RDPluginHeader*>(passembler), m_assemblers); }
bool Context::registerPlugin(RDCommandPlugin* pcommand) { return this->registerPlugin(reinterpret_cast<RDPluginHeader*>(pcommand), m_commands);  }

bool Context::commandExecute(const char* command, const RDArguments* arguments)
{
    auto it = m_commands.find(command);

    if((it == m_commands.end()) || !it->second)
    {
        this->log("Cannot find command " + Utils::quoted(command));
        return false;
    }

    RDCommandPlugin* pcommand = reinterpret_cast<RDCommandPlugin*>(std::addressof(it->second));

    if(!pcommand->execute(pcommand, arguments))
    {
        this->log("Command execution " + Utils::quoted(command) + " failed");
        return false;
    }

    return true;
}

void Context::getLoaders(const RDLoaderRequest* loadrequest, Callback_LoaderPlugin callback, void* userdata)
{
    m_loadertoassembler.clear();
    if(!callback) return;

    for(const auto& item : m_loaders)
    {
        RDLoaderPlugin* ploader = reinterpret_cast<RDLoaderPlugin*>(item.second);

        Context::initPlugin(reinterpret_cast<RDPluginHeader*>(ploader));
        RDAssemblerPlugin* passembler = ploader->test(ploader, loadrequest);

        if(!passembler)
        {
            Context::freePlugin(reinterpret_cast<RDPluginHeader*>(ploader));
            continue;
        }

        m_loadertoassembler[ploader] = passembler; // Save proposed assembler
        callback(ploader, userdata);
    }
}

void Context::getAssemblers(Callback_AssemblerPlugin callback, void* userdata)
{
    if(!callback) return;

    for(const auto& item : m_assemblers)
    {
        Context::initPlugin(item.second);
        callback(reinterpret_cast<RDAssemblerPlugin*>(item.second), userdata);
    }
}

RDAssemblerPlugin* Context::getAssembler(const RDLoaderPlugin* ploader) const
{
    auto it = m_loadertoassembler.find(ploader);
    return (it != m_loadertoassembler.end()) ? reinterpret_cast<RDAssemblerPlugin*>(it->second) : nullptr;
}

RDAssemblerPlugin* Context::findAssembler(const char* id) const
{
    auto it = m_assemblers.find(id);
    return (it != m_assemblers.end()) ? reinterpret_cast<RDAssemblerPlugin*>(it->second) : nullptr;
}

void Context::setDisassembler(Disassembler* disassembler)
{
    m_disassembler = disassembler;
    m_problems.clear(); // New disassembler, New problems
}

void Context::setRuntimePath(const char* rntpath) { m_rntpath = rntpath; }
void Context::setTempPath(const char* tmppath) { m_tmppath = tmppath; }
void Context::setIgnoreProblems(bool ignore) { m_ignoreproblems = ignore; }
Context* Context::instance() { static Context context; return &context; }
void Context::setFlags(flag_t flag) { m_flags = flag; }
flag_t Context::flags() const { return m_flags; }
bool Context::hasProblems() const { return !m_problems.empty(); }
bool Context::sync() const { return m_sync; }
size_t Context::problemsCount() const { return m_problems.size(); }
PluginManager* Context::pluginManager() { return &m_pluginmanager; }
Disassembler* Context::disassembler() const { return m_disassembler; }
const Context::StringSet& Context::databasePaths() const { return m_dbpaths; }
const char* Context::runtimePath() const { return m_rntpath.c_str(); }
const char* Context::tempPath() const { return m_tmppath.c_str(); }
void Context::status(const std::string& s) { this->status(s.c_str()); }
void Context::log(const std::string& s) { this->log(s.c_str()); }

void Context::getProblems(RD_ProblemCallback callback, void* userdata)
{
    for(const std::string& problem : m_problems)
        callback(problem.c_str(), userdata);
}

void Context::problem(const std::string& s)
{
    if(m_ignoreproblems) return;

    log_lock lock(m_mutex);
    m_problems.insert(s);
}

void Context::init()
{
    for(const std::string& pluginpath : m_pluginpaths)
        m_pluginmanager.loadAll(pluginpath);
}

void Context::initPlugin(RDPluginHeader* plugin)
{
    if(plugin->state == PluginState_Initialized) Context::freePlugin(plugin);

    if(plugin->init) plugin->init(plugin);
    plugin->state = PluginState_Initialized;
}

void Context::freePlugin(RDPluginHeader* plugin)
{
    if(plugin->state != PluginState_Initialized) return;

    if(plugin->free) plugin->free(plugin);
    plugin->state = PluginState_Loaded;
}

bool Context::registerPlugin(RDPluginHeader* plugin, PluginMap& pluginmap)
{
    if(!plugin->name || !std::strlen(plugin->name))
    {
        this->log("Invalid plugin name");
        return false;
    }

    auto it = pluginmap.find(plugin->id);

    if(it != pluginmap.end())
    {
        this->log("Plugin " + Utils::quoted(it->first) + " already exists");
        return false;
    }

    pluginmap[plugin->id] = plugin;
    return true;
}
