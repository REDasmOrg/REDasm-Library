#pragma once

#include <unordered_set>
#include <string>
#include <chrono>
#include <mutex>
#include <list>
#include <rdapi/context.h>
#include <rdapi/plugin/loader.h>
#include <rdapi/plugin/assembler.h>
#include <rdapi/plugin/command.h>
#include "plugin/interface/pluginmanager.h"
#include "object.h"

template<typename Callback>
struct CallbackStruct
{
    Callback callback{nullptr};
    void* userdata{nullptr};
};

class Disassembler;

class Context: public Object
{
    private:
        typedef std::unordered_set<std::string> StringSet;
        typedef std::unordered_map<const RDLoaderPlugin*, const char*> LoaderToAssemblerMap;
        typedef std::unordered_map<std::string, RDPluginHeader*> PluginMap;
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

    public:
        void statusProgress(const char* s, size_t progress) const;
        void statusAddress(const char* s, address_t address) const;
        void status(const char* s) const;
        void log(const char* s) const;

    public: // Plugin
        bool registerPlugin(RDLoaderPlugin* ploader);
        bool registerPlugin(RDAssemblerPlugin* passembler);
        bool registerPlugin(RDCommandPlugin* pcommand);
        bool commandExecute(const char* command, const RDArguments* arguments);
        void getLoaders(const RDLoaderRequest* loadrequest, Callback_LoaderPlugin callback, void* userdata);
        void getAssemblers(Callback_AssemblerPlugin callback, void* userdata);
        const char* getAssemblerId(const RDLoaderPlugin* ploader) const;
        RDAssemblerPlugin* getAssembler(const RDLoaderPlugin* ploader) const;
        RDAssemblerPlugin* findAssembler(const char* id) const;

    public:
        static Context* instance();
        static void freePlugin(RDPluginHeader* plugin);

    public:
        void setFlags(flag_t flag);
        flag_t flags() const;
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
        void problem(const std::string& s);
        void init();

    private:
        static void initPlugin(RDPluginHeader* plugin);
        bool registerPlugin(RDPluginHeader* plugin, PluginMap& pluginmap);

    private:
        Disassembler* m_disassembler{nullptr};
        flag_t m_flags{ContextFlag_None};
        PluginManager m_pluginmanager;
        PluginMap m_loaders, m_assemblers, m_commands;
        LoaderToAssemblerMap m_loadertoassembler;
        CallbackStruct<RD_LogCallback> m_logcallback;
        CallbackStruct<RD_StatusCallback> m_statuscallback;
        CallbackStruct<RD_ProgressCallback> m_progresscallback;
        StringSet m_pluginpaths, m_dbpaths, m_problems;
        mutable std::chrono::steady_clock::time_point m_laststatusreport;
        std::chrono::milliseconds m_debouncetimeout;
        std::string m_rntpath, m_tmppath;
        mutable std::mutex m_mutex;
        bool m_ignoreproblems{false};
};

#define rd_ctx    Context::instance()
#define rd_pm     rd_ctx->pluginManager()
#define rd_disasm rd_ctx->disassembler()
#define rd_asm    rd_disasm->assembler()
#define rd_ldr    rd_disasm->loader()
#define rd_doc    rd_ldr->document()
