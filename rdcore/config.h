#pragma once

#include <rdapi/config.h>
#include <filesystem>
#include <utility>
#include <string>
#include <chrono>
#include <mutex>
#include "containers/uniquecontainer.h"

#define DATABASE_FOLDER_NAME "database"

namespace fs = std::filesystem;
typedef fs::path::string_type platform_string;

struct RDUI;

template<typename Callback>
struct CallbackStruct
{
    Callback callback{nullptr};
    void* userdata{nullptr};

    template<typename Args> void operator()(Args&& args) const { callback(std::forward<Args>(args), userdata); }
};

struct ThemeColors
{
    std::string fg{"#000000"}, bg{"#ffffff"}, seek;
    std::string comment, autocomment;
    std::string highlightfg, highlightbg;
    std::string selectionfg, selectionbg;
    std::string cursorfg, cursorbg;
    std::string segment, function, type;
    std::string address, constant, reg;
    std::string string, symbol, data, pointer, imported;
    std::string nop, ret, call, jump, jumpcond;
    std::string entryfg, entrybg;
    std::string statetrue, statefalse;
    std::string graphbg, graphedge, graphedgeloop, graphedgeloopcond;
};

class Config
{
    private:
        using log_lock = std::scoped_lock<std::mutex>;

    public:
        Config();
        static Config* instance();
        void addPluginPath(const char* pluginpath);
        void addDatabasePath(const char* dbpath);
        void setRuntimePath(const char* rntpath);
        void setTempPath(const char* tmppath);
        void setUI(const RDUI* ui);
        void setTheme(rd_type theme, const char* color);
        void setLogCallback(RD_LogCallback callback, void* userdata);
        void setStatusCallback(RD_StatusCallback callback, void* userdata);
        void setProgressCallback(RD_ProgressCallback callback, void* userdata);
        void getDatabasePaths(RD_PathCallback callback, void* userdata) const;
        void getPluginPaths(RD_PathCallback callback, void* userdata) const;
        const CallbackStruct<RD_ProgressCallback>& progressCallback() const;
        const CallbackStruct<RD_StatusCallback>& statusCallback() const;
        const CallbackStruct<RD_LogCallback>& logCallback() const;
        const UniqueContainer<fs::path>& databasePaths() const;
        const UniqueContainer<fs::path>& pluginPaths() const;
        std::string runtimePath() const;
        std::string tempPath() const;
        const RDUI* ui() const;
        const char* theme(rd_type theme) const;

    public:
        void statusProgress(const char* s, size_t progress) const;
        void statusAddress(const char* s, rd_address address) const;
        void status(const char* s) const;
        void log(const char* s) const;
        inline void status(const std::string& s) const { this->status(static_cast<const char*>(s.c_str())); }
        inline void log(const std::string& s) const { this->log(s.c_str()); }

    private:
        static const char* themeAlt(const std::string& color, const std::string& altcolor);

    private:
        UniqueContainer<fs::path> m_pluginpaths, m_dbpaths;
        CallbackStruct<RD_ProgressCallback> m_progresscallback;
        CallbackStruct<RD_StatusCallback> m_statuscallback;
        CallbackStruct<RD_LogCallback> m_logcallback;
        fs::path m_rntpath, m_tmppath;
        ThemeColors m_themecolors;
        const RDUI* m_ui{nullptr};

    private:
        std::chrono::milliseconds m_debouncetimeout;
        mutable std::chrono::steady_clock::time_point m_laststatusreport;
        mutable std::mutex m_mutex;
};

#define rd_cfg Config::instance()
