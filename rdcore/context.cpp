#include "context.h"
#include "support/utils.h"
#include "eventdispatcher.h"
#include <rdapi/theme.h>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <cstring>

#define CONTEXT_DEBOUNCE_TIMEOUT_MS 100

#define CONTEXT_DEBOUNCE_CHECK \
    auto now = std::chrono::steady_clock::now(); \
    if((now - m_laststatusreport) < m_debouncetimeout) return; \
    m_laststatusreport = now;

Context::Context(): m_rntpath(std::filesystem::current_path()), m_tmppath(std::filesystem::temp_directory_path()) { EventDispatcher::initialize(); }
Context::~Context() { EventDispatcher::deinitialize(); }
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

void Context::setTheme(rd_type theme, const char* color)
{
    switch(theme)
    {
        case Theme_Foreground:        m_themecolors.fg = color; break;
        case Theme_Background:        m_themecolors.bg = color; break;
        case Theme_Seek:              m_themecolors.seek = color; break;
        case Theme_Comment:           m_themecolors.comment = color; break;
        case Theme_Meta:              m_themecolors.meta = color; break;
        case Theme_HighlightFg:       m_themecolors.highlightfg = color; break;
        case Theme_HighlightBg:       m_themecolors.highlightbg = color; break;
        case Theme_SelectionFg:       m_themecolors.selectionfg = color; break;
        case Theme_SelectionBg:       m_themecolors.selectionbg = color; break;
        case Theme_CursorFg:          m_themecolors.cursorfg = color; break;
        case Theme_CursorBg:          m_themecolors.cursorbg = color; break;
        case Theme_Segment:           m_themecolors.segment = color; break;
        case Theme_Function:          m_themecolors.function = color; break;
        case Theme_Type:              m_themecolors.type = color; break;
        case Theme_Address:           m_themecolors.address = color; break;
        case Theme_Constant:          m_themecolors.constant = color; break;
        case Theme_Reg:               m_themecolors.reg = color; break;
        case Theme_String:            m_themecolors.string = color; break;
        case Theme_Symbol:            m_themecolors.symbol = color; break;
        case Theme_Data:              m_themecolors.data = color; break;
        case Theme_Imported:          m_themecolors.imported = color; break;
        case Theme_Nop:               m_themecolors.nop = color; break;
        case Theme_Ret:               m_themecolors.ret = color; break;
        case Theme_Call:              m_themecolors.call = color; break;
        case Theme_Jump:              m_themecolors.jump = color; break;
        case Theme_JumpCond:          m_themecolors.jumpcond = color; break;
        case Theme_GraphBg:           m_themecolors.graphbg = color; break;
        case Theme_GraphEdge:         m_themecolors.graphedge = color; break;
        case Theme_GraphEdgeTrue:     m_themecolors.graphedgetrue = color; break;
        case Theme_GraphEdgeFalse:    m_themecolors.graphedgefalse = color; break;
        case Theme_GraphEdgeLoop:     m_themecolors.graphedgeloop = color; break;
        case Theme_GraphEdgeLoopCond: m_themecolors.graphedgeloopcond = color; break;
        default: break;
    }
}

const char* Context::getTheme(rd_type theme) const
{
    switch(theme)
    {
        case Theme_Foreground:        return m_themecolors.fg.c_str();
        case Theme_Background:        return m_themecolors.bg.c_str();
        case Theme_Seek:              return Context::themeAlt(m_themecolors.seek, m_themecolors.bg);
        case Theme_Comment:           return Context::themeAlt(m_themecolors.comment, m_themecolors.fg);
        case Theme_Meta:              return Context::themeAlt(m_themecolors.meta, m_themecolors.fg);
        case Theme_HighlightFg:       return Context::themeAlt(m_themecolors.highlightfg, m_themecolors.fg);
        case Theme_HighlightBg:       return Context::themeAlt(m_themecolors.highlightbg, m_themecolors.bg);
        case Theme_SelectionFg:       return Context::themeAlt(m_themecolors.selectionfg, m_themecolors.fg);
        case Theme_SelectionBg:       return Context::themeAlt(m_themecolors.selectionbg, m_themecolors.bg);
        case Theme_CursorFg:          return Context::themeAlt(m_themecolors.cursorfg, m_themecolors.fg);
        case Theme_CursorBg:          return Context::themeAlt(m_themecolors.cursorbg, m_themecolors.bg);
        case Theme_Segment:           return Context::themeAlt(m_themecolors.segment, m_themecolors.fg);
        case Theme_Function:          return Context::themeAlt(m_themecolors.function, m_themecolors.fg);
        case Theme_Type:              return Context::themeAlt(m_themecolors.type, m_themecolors.fg);
        case Theme_Address:           return Context::themeAlt(m_themecolors.address, m_themecolors.fg);
        case Theme_String:            return Context::themeAlt(m_themecolors.string, m_themecolors.fg);
        case Theme_Symbol:            return Context::themeAlt(m_themecolors.symbol, m_themecolors.fg);
        case Theme_Data:              return Context::themeAlt(m_themecolors.data, m_themecolors.fg);
        case Theme_Imported:          return Context::themeAlt(m_themecolors.imported, m_themecolors.fg);
        case Theme_Constant:          return Context::themeAlt(m_themecolors.constant, m_themecolors.fg);
        case Theme_Reg:               return Context::themeAlt(m_themecolors.reg, m_themecolors.fg);
        case Theme_Nop:               return Context::themeAlt(m_themecolors.nop, m_themecolors.fg);
        case Theme_Ret:               return Context::themeAlt(m_themecolors.ret, m_themecolors.fg);
        case Theme_Call:              return Context::themeAlt(m_themecolors.call, m_themecolors.fg);
        case Theme_Jump:              return Context::themeAlt(m_themecolors.jump, m_themecolors.fg);
        case Theme_JumpCond:          return Context::themeAlt(m_themecolors.jumpcond, m_themecolors.fg);
        case Theme_GraphBg:           return Context::themeAlt(m_themecolors.graphbg, m_themecolors.bg);
        case Theme_GraphEdge:         return Context::themeAlt(m_themecolors.graphedge, m_themecolors.fg);
        case Theme_GraphEdgeTrue:     return Context::themeAlt(m_themecolors.graphedgetrue, m_themecolors.fg);
        case Theme_GraphEdgeFalse:    return Context::themeAlt(m_themecolors.graphedgefalse, m_themecolors.fg);
        case Theme_GraphEdgeLoop:     return Context::themeAlt(m_themecolors.graphedgeloop, m_themecolors.fg);
        case Theme_GraphEdgeLoopCond: return Context::themeAlt(m_themecolors.graphedgeloopcond, m_themecolors.fg);
        default: break;
    }

    return nullptr;
}

void Context::statusProgress(const char* s, size_t progress) const
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

void Context::statusAddress(const char* s, rd_address address) const
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

void Context::status(const char* s) const
{
    log_lock lock(m_mutex);
    CONTEXT_DEBOUNCE_CHECK

    if(m_statuscallback.callback)
        m_statuscallback.callback(s, m_statuscallback.userdata);
    else
        std::cout << s << std::endl;
}

void Context::log(const char* s) const
{
    log_lock lock(m_mutex);
    CONTEXT_DEBOUNCE_CHECK

    if(m_logcallback.callback)
        m_logcallback.callback(s, m_logcallback.userdata);
    else
        std::cout << s << std::endl;
}

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
    if(!callback || !loadrequest->filepath || !loadrequest->buffer) return;

    for(const auto& item : m_loaders)
    {
        RDLoaderPlugin* ploader = reinterpret_cast<RDLoaderPlugin*>(item.second);

        Context::initPlugin(reinterpret_cast<RDPluginHeader*>(ploader));
        const char* assemblerid = ploader->test(ploader, loadrequest);

        if(!assemblerid)
        {
            Context::freePlugin(reinterpret_cast<RDPluginHeader*>(ploader));
            continue;
        }

        m_loadertoassembler[ploader] = assemblerid; // Save proposed assembler
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

const char* Context::getAssemblerId(const RDLoaderPlugin* ploader) const
{
    auto it = m_loadertoassembler.find(ploader);
    return (it != m_loadertoassembler.end()) ? it->second : nullptr;
}

RDAssemblerPlugin* Context::getAssembler(const RDLoaderPlugin* ploader) const
{
    auto it = m_loadertoassembler.find(ploader);
    if(it == m_loadertoassembler.end()) return nullptr;

    RDAssemblerPlugin* passembler = this->findAssembler(it->second);
    if(!passembler) this->log("Cannot find assembler '" + std::string(it->second) + "'");
    Context::initPlugin(reinterpret_cast<RDPluginHeader*>(passembler));
    return passembler;
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

void Context::setFlags(rd_flag flags, bool set)
{
    rd_flag oldflags = m_flags;

    if(set) m_flags |= flags;
    else m_flags &= ~flags;

    if(m_flags != oldflags)
        EventDispatcher::enqueue<RDEventArgs>(RDEvents::Event_ContextFlagsChanged, this);
}

rd_flag Context::flags() const { return m_flags; }
bool Context::hasFlags(rd_flag flags) const { return m_flags & flags; }
bool Context::hasProblems() const { return !m_problems.empty(); }
size_t Context::problemsCount() const { return m_problems.size(); }
PluginManager* Context::pluginManager() { return &m_pluginmanager; }
Disassembler* Context::disassembler() const { return m_disassembler; }
const Context::StringSet& Context::databasePaths() const { return m_dbpaths; }
const char* Context::runtimePath() const { return m_rntpath.c_str(); }
const char* Context::tempPath() const { return m_tmppath.c_str(); }
void Context::status(const std::string& s) const { this->status(static_cast<const char*>(s.c_str())); }
void Context::log(const std::string& s) const { this->log(s.c_str()); }
void Context::getProblems(RD_ProblemCallback callback, void* userdata) const { for(const std::string& problem : m_problems) callback(problem.c_str(), userdata); }
void Context::getPluginPaths(RD_PluginCallback callback, void* userdata) const { for(const std::string& pluginpath : m_pluginpaths) callback(pluginpath.c_str(), userdata); }

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

const char* Context::themeAlt(const std::string& color, const std::string& altcolor) { return color.empty() ? altcolor.c_str() : color.c_str(); }

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

void Context::setUI(const RDUI* rdui) { m_ui = rdui; }
const RDUI* Context::ui() const { return m_ui; }
void Context::initFlags(rd_flag flags) { m_flags = flags; }

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
