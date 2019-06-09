#pragma once

#include <functional>
#include <list>
#include "plugins/pluginmanager.h"
#include "plugins/loader/loader.h"
#include "support/path.h"
#include "pimpl.h"
#include "ui.h"

namespace REDasm {

typedef std::function<void(const std::string&)> Context_LogCallback;
typedef std::function<void(size_t)> Context_ProgressCallback;
typedef std::deque<std::string> ProblemList;
typedef std::list<std::string> PluginPaths;

struct LIBREDASM_API ContextSettings
{
    ContextSettings(): ignoreproblems(false) { }

    std::string runtimePath, tempPath;
    Context_LogCallback logCallback;
    Context_LogCallback statusCallback;
    Context_ProgressCallback progressCallback;
    std::shared_ptr<AbstractUI> ui;
    PluginPaths pluginPaths;
    bool ignoreproblems;
};

class ContextImpl;

class LIBREDASM_API Context
{
    PIMPL_DECLARE_PRIVATE(Context)

    private:
        Context();

    public:
        static void init(Context *ctx);
        static Context* init(const ContextSettings& settings);
        static Context* instance();

    public:
        bool hasProblems() const;
        size_t problemsCount() const;
        const ProblemList& problems() const;
        void clearProblems();

    public:
        void cwd(const std::string& s);
        void sync(bool b);
        void log(const std::string& s);
        void problem(const std::string& s);
        void logproblem(const std::string& s);
        void status(const std::string& s);
        void statusProgress(const std::string& s, size_t progress);
        void statusAddress(const std::string& s, address_t address);
        bool sync();
        PluginManager* pluginManager() const;
        const PluginPaths& pluginPaths() const;
        std::string capstoneVersion() const;
        std::string runtimePath() const;
        std::string tempPath() const;

    public:
        template<typename... T> inline std::string rnt(const std::string& p, T... args) const { return Path::create(this->runtimePath(), p, args...); }
        template<typename... T> inline std::string db(const std::string& p, T... args) const { return this->rnt("database", p, args...); }
        template<typename... T> inline std::string signature(const std::string& p, T... args) const { return this->db("signatures", p, args...); }
};

} // namespace REDasm

#define r_ctx REDasm::Context::instance()
#define r_pm  r_ctx->pluginManager()

