#include "config.h"
#include "support/utils.h"
#include <rdapi/ui.h>
#include <filesystem>
#include <iostream>
#include <sstream>

#define CONTEXT_DEBOUNCE_TIMEOUT_MS 100

#define CONTEXT_DEBOUNCE_CHECK \
    auto now = std::chrono::steady_clock::now(); \
    if((now - m_laststatusreport) < m_debouncetimeout) return; \
    m_laststatusreport = now;

namespace fs = std::filesystem;

Config::Config(): m_rntpath(fs::current_path()), m_tmppath(fs::temp_directory_path())
{
    m_dbpaths.insert(fs::path(m_rntpath) / DATABASE_FOLDER_NAME);

    m_progresscallback.callback = [](size_t, void*) { };
    m_progresscallback.userdata = nullptr;

    m_logcallback.callback = [](const char* s, void*) { std::cout << s << std::endl; };
    m_logcallback.userdata = nullptr;

    m_statuscallback.callback = [](const char* s, void*) { std::cout << s << std::endl; };
    m_statuscallback.userdata = nullptr;
}

Config* Config::instance() { static Config config; return &config; }
void Config::addPluginPath(const char* pluginpath) { m_pluginpaths.insert(pluginpath); }
void Config::addDatabasePath(const char* dbpath) { m_dbpaths.insert(dbpath); }

void Config::setRuntimePath(const char* rntpath)
{
    m_dbpaths.remove(fs::path(m_rntpath) / DATABASE_FOLDER_NAME);
    m_rntpath = rntpath;
    m_dbpaths.insert(fs::path(m_rntpath) / DATABASE_FOLDER_NAME);
}

void Config::setTempPath(const char* tmppath) { m_tmppath = tmppath; }
void Config::setUI(const RDUI* ui) { m_ui = ui; }

void Config::setTheme(rd_type theme, const char* color)
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
        case Theme_Pointer:           m_themecolors.pointer = color; break;
        case Theme_Imported:          m_themecolors.imported = color; break;
        case Theme_Nop:               m_themecolors.nop = color; break;
        case Theme_Ret:               m_themecolors.ret = color; break;
        case Theme_Call:              m_themecolors.call = color; break;
        case Theme_Jump:              m_themecolors.jump = color; break;
        case Theme_JumpCond:          m_themecolors.jumpcond = color; break;
        case Theme_EntryFg:           m_themecolors.entryfg = color; break;
        case Theme_EntryBg:           m_themecolors.entrybg = color; break;
        case Theme_GraphBg:           m_themecolors.graphbg = color; break;
        case Theme_GraphEdge:         m_themecolors.graphedge = color; break;
        case Theme_GraphEdgeTrue:     m_themecolors.graphedgetrue = color; break;
        case Theme_GraphEdgeFalse:    m_themecolors.graphedgefalse = color; break;
        case Theme_GraphEdgeLoop:     m_themecolors.graphedgeloop = color; break;
        case Theme_GraphEdgeLoopCond: m_themecolors.graphedgeloopcond = color; break;
        default: break;
    }
}

void Config::setLogCallback(RD_LogCallback callback, void* userdata)
{
    m_logcallback.callback = callback;
    m_logcallback.userdata = userdata;
}

void Config::setStatusCallback(RD_StatusCallback callback, void* userdata)
{
    m_statuscallback.callback = callback;
    m_statuscallback.userdata = userdata;
}

void Config::setProgressCallback(RD_ProgressCallback callback, void* userdata)
{
    m_progresscallback.callback = callback;
    m_progresscallback.userdata = userdata;
}

const char* Config::theme(rd_type theme) const
{
    switch(theme)
    {
        case Theme_Foreground:        return m_themecolors.fg.c_str();
        case Theme_Background:        return m_themecolors.bg.c_str();
        case Theme_Seek:              return Config::themeAlt(m_themecolors.seek, m_themecolors.bg);
        case Theme_Comment:           return Config::themeAlt(m_themecolors.comment, m_themecolors.fg);
        case Theme_Meta:              return Config::themeAlt(m_themecolors.meta, m_themecolors.fg);
        case Theme_HighlightFg:       return Config::themeAlt(m_themecolors.highlightfg, m_themecolors.fg);
        case Theme_HighlightBg:       return Config::themeAlt(m_themecolors.highlightbg, m_themecolors.bg);
        case Theme_SelectionFg:       return Config::themeAlt(m_themecolors.selectionfg, m_themecolors.fg);
        case Theme_SelectionBg:       return Config::themeAlt(m_themecolors.selectionbg, m_themecolors.bg);
        case Theme_CursorFg:          return Config::themeAlt(m_themecolors.cursorfg, m_themecolors.fg);
        case Theme_CursorBg:          return Config::themeAlt(m_themecolors.cursorbg, m_themecolors.bg);
        case Theme_Segment:           return Config::themeAlt(m_themecolors.segment, m_themecolors.fg);
        case Theme_Function:          return Config::themeAlt(m_themecolors.function, m_themecolors.fg);
        case Theme_Type:              return Config::themeAlt(m_themecolors.type, m_themecolors.fg);
        case Theme_Address:           return Config::themeAlt(m_themecolors.address, m_themecolors.fg);
        case Theme_String:            return Config::themeAlt(m_themecolors.string, m_themecolors.fg);
        case Theme_Symbol:            return Config::themeAlt(m_themecolors.symbol, m_themecolors.fg);
        case Theme_Data:              return Config::themeAlt(m_themecolors.data, m_themecolors.fg);
        case Theme_Pointer:           return Config::themeAlt(m_themecolors.pointer, m_themecolors.fg);
        case Theme_Imported:          return Config::themeAlt(m_themecolors.imported, m_themecolors.fg);
        case Theme_Constant:          return Config::themeAlt(m_themecolors.constant, m_themecolors.fg);
        case Theme_Reg:               return Config::themeAlt(m_themecolors.reg, m_themecolors.fg);
        case Theme_Nop:               return Config::themeAlt(m_themecolors.nop, m_themecolors.fg);
        case Theme_Ret:               return Config::themeAlt(m_themecolors.ret, m_themecolors.fg);
        case Theme_Call:              return Config::themeAlt(m_themecolors.call, m_themecolors.fg);
        case Theme_Jump:              return Config::themeAlt(m_themecolors.jump, m_themecolors.fg);
        case Theme_JumpCond:          return Config::themeAlt(m_themecolors.jumpcond, m_themecolors.fg);
        case Theme_EntryFg:           return Config::themeAlt(m_themecolors.entryfg, m_themecolors.fg);
        case Theme_EntryBg:           return Config::themeAlt(m_themecolors.entrybg, m_themecolors.bg);
        case Theme_GraphBg:           return Config::themeAlt(m_themecolors.graphbg, m_themecolors.bg);
        case Theme_GraphEdge:         return Config::themeAlt(m_themecolors.graphedge, m_themecolors.fg);
        case Theme_GraphEdgeTrue:     return Config::themeAlt(m_themecolors.graphedgetrue, m_themecolors.fg);
        case Theme_GraphEdgeFalse:    return Config::themeAlt(m_themecolors.graphedgefalse, m_themecolors.fg);
        case Theme_GraphEdgeLoop:     return Config::themeAlt(m_themecolors.graphedgeloop, m_themecolors.fg);
        case Theme_GraphEdgeLoopCond: return Config::themeAlt(m_themecolors.graphedgeloopcond, m_themecolors.fg);
        default: break;
    }

    return nullptr;
}

void Config::statusProgress(const char* s, size_t progress) const
{
    log_lock lock(m_mutex);

    if(progress) {
        CONTEXT_DEBOUNCE_CHECK
    }

    m_statuscallback(s);
    m_progresscallback(progress);
}

void Config::statusAddress(const char* s, rd_address address) const
{
    log_lock lock(m_mutex);
    CONTEXT_DEBOUNCE_CHECK

    std::stringstream ss;
    ss << s << " @ " << Utils::hex(address);
    m_statuscallback(ss.str().c_str());
}

void Config::status(const char* s) const
{
    log_lock lock(m_mutex);
    CONTEXT_DEBOUNCE_CHECK
    m_statuscallback(s);
}

void Config::log(const char* s) const
{
    log_lock lock(m_mutex);
    CONTEXT_DEBOUNCE_CHECK
    m_logcallback(s);
}

void Config::getDatabasePaths(RD_PathCallback callback, void* userdata) const { for(const std::string& dbpath : m_dbpaths) callback(dbpath.c_str(), userdata); }
void Config::getPluginPaths(RD_PathCallback callback, void* userdata) const { for(const std::string& pluginpath : m_pluginpaths) callback(pluginpath.c_str(), userdata); }
const CallbackStruct<RD_ProgressCallback>& Config::progressCallback() const { return m_progresscallback; }
const CallbackStruct<RD_StatusCallback>& Config::statusCallback() const { return m_statuscallback; }
const CallbackStruct<RD_LogCallback>& Config::logCallback() const { return m_logcallback; }
const UniqueContainer<std::string>& Config::databasePaths() const { return m_dbpaths; }
const UniqueContainer<std::string>& Config::pluginPaths() const { return m_pluginpaths; }
const char* Config::runtimePath() const { return m_rntpath.c_str(); }
const char* Config::tempPath() const { return m_tmppath.c_str(); }
const RDUI* Config::ui() const { return m_ui; }
const char* Config::themeAlt(const std::string& color, const std::string& altcolor) { return color.empty() ? altcolor.c_str() : color.c_str(); }
