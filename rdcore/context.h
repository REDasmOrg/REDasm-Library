#pragma once

#include <unordered_set>
#include <string>
#include <chrono>
#include <mutex>
#include <list>
#include <rdapi/context.h>
#include <rdapi/ui.h>
#include <rdapi/plugin/loader.h>
#include <rdapi/plugin/analyzer.h>
#include <rdapi/plugin/command.h>
#include <rdapi/plugin/assembler/assembler.h>
#include "plugin/interface/pluginmanager.h"
#include "containers/sortedcontainer.h"
#include "object.h"

template<typename Callback>
struct CallbackStruct
{
    Callback callback{nullptr};
    void* userdata{nullptr};
};

struct AnalyzerSorter { bool operator ()(const RDAnalyzerPlugin* a1, const RDAnalyzerPlugin* a2) const { return a1->priority > a2->priority; } };
struct AnalyzerEquals { bool operator ()(const RDAnalyzerPlugin* a1, const RDAnalyzerPlugin* a2) const { return a1->execute == a2->execute; } };

struct ThemeColors {
    std::string fg{"#000000"}, bg{"#ffffff"}, seek, comment, meta;
    std::string highlightfg, highlightbg;
    std::string selectionfg, selectionbg;
    std::string cursorfg, cursorbg;
    std::string segment, function, type;
    std::string address, constant, reg;
    std::string string, symbol, data, imported;
    std::string nop, ret, call, jump, jumpcond;
    std::string entryfg, entrybg;
    std::string graphbg, graphedge, graphedgetrue, graphedgefalse, graphedgeloop, graphedgeloopcond;
};

class Disassembler;

class Context: public Object
{
    private:
        typedef std::unordered_set<std::string> StringSet;
        typedef std::unordered_map<const RDLoaderPlugin*, const char*> LoaderToAssemblerMap;
        typedef std::unordered_map<std::string, RDPluginHeader*> PluginMap;
        typedef SortedContainer<RDAnalyzerPlugin*, AnalyzerSorter, AnalyzerEquals, true> AnalyzerList;
        using log_lock = std::scoped_lock<std::mutex>;

    public:
        Context();
        virtual ~Context();
        void addPluginPath(const char* pluginpath);
        void addDatabasePath(const char* dbpath);
        void setDisassembler(Disassembler* disassembler);
        void setRuntimePath(const char* rntpath);
        void setTempPath(const char* tmppath);
        void setIgnoreProblems(bool ignore);
        void setLogCallback(RD_LogCallback callback, void* userdata);
        void setStatusCallback(RD_StatusCallback callback, void* userdata);
        void setProgressCallback(RD_ProgressCallback callback, void* userdata);
        void setTheme(rd_type theme, const char* color);
        const char* getTheme(rd_type theme) const;
        const AnalyzerList& selectedAnalyzers() const;

    public:
        void statusProgress(const char* s, size_t progress) const;
        void statusAddress(const char* s, rd_address address) const;
        void status(const char* s) const;
        void log(const char* s) const;

    public: // Plugin
        bool registerPlugin(RDLoaderPlugin* ploader);
        bool registerPlugin(RDAssemblerPlugin* passembler);
        bool registerPlugin(RDAnalyzerPlugin* panalyzer);
        bool registerPlugin(RDCommandPlugin* pcommand);
        bool commandExecute(const char* command, const RDArguments* arguments);
        void getAnalyzers(const RDLoaderPlugin* loader, const RDAssemblerPlugin* assembler, Callback_AnalyzerPlugin callback, void* userdata);
        void getLoaders(const RDLoaderRequest* loadrequest, Callback_LoaderPlugin callback, void* userdata);
        void getAssemblers(Callback_AssemblerPlugin callback, void* userdata);
        const char* getAssemblerId(const RDLoaderPlugin* ploader) const;
        RDAssemblerPlugin* getAssembler(const RDLoaderPlugin* ploader) const;
        RDAssemblerPlugin* findAssembler(const char* id) const;
        void selectAnalyzer(RDAnalyzerPlugin* panalyzer, bool selected);

    public:
        static Context* instance();
        static void freePlugin(RDPluginHeader* plugin);

    public:
        void setUI(const RDUI* rdui);
        const RDUI* ui() const;
        void initFlags(rd_flag flags);
        void setFlags(rd_flag flags, bool set);
        rd_flag flags() const;
        bool hasFlags(rd_flag flags) const;
        bool hasProblems() const;
        size_t problemsCount() const;
        PluginManager* pluginManager();
        Disassembler* disassembler() const;
        const StringSet& databasePaths() const;
        const char* runtimePath() const;
        const char* tempPath() const;
        void status(const std::string& s) const;
        void log(const std::string& s) const;
        void getProblems(RD_ProblemCallback callback, void* userdata) const;
        void getPluginPaths(RD_PluginCallback callback, void* userdata) const;
        void problem(const std::string& s);
        void init();

    private:
        static const char* themeAlt(const std::string& color, const std::string& altcolor);
        static void initPlugin(RDPluginHeader* plugin);
        bool registerPlugin(RDPluginHeader* plugin, PluginMap& pluginmap);
        void initBuiltins();

    private:
        Disassembler* m_disassembler{nullptr};
        rd_flag m_flags{ContextFlags_None};
        PluginManager m_pluginmanager;
        PluginMap m_loaders, m_assemblers, m_analyzers, m_commands;
        LoaderToAssemblerMap m_loadertoassembler;
        AnalyzerList m_selectedanalyzers;
        CallbackStruct<RD_LogCallback> m_logcallback;
        CallbackStruct<RD_StatusCallback> m_statuscallback;
        CallbackStruct<RD_ProgressCallback> m_progresscallback;
        StringSet m_pluginpaths, m_dbpaths, m_problems;
        mutable std::chrono::steady_clock::time_point m_laststatusreport;
        std::chrono::milliseconds m_debouncetimeout;
        std::string m_rntpath, m_tmppath;
        ThemeColors m_themecolors;
        mutable std::mutex m_mutex;
        bool m_ignoreproblems{false};
        const RDUI* m_ui{nullptr};
};

#define rd_ctx    Context::instance()
#define rd_pm     rd_ctx->pluginManager()
#define rd_disasm rd_ctx->disassembler()
#define rd_ui     rd_ctx->ui()
