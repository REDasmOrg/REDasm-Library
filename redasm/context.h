#pragma once

#include <functional>
#include <list>
#include "plugins/pluginmanager.h"
#include "plugins/loader/loader.h"
#include "support/path.h"
#include "pimpl.h"
#include "ui.h"

namespace REDasm {

typedef std::function<void(const String&)> Context_LogCallback;
typedef std::function<void(size_t)> Context_ProgressCallback;
typedef std::deque<String> ProblemList;
typedef std::list<String> PluginPaths;

struct LIBREDASM_API ContextSettings
{
    ContextSettings(): ignoreproblems(false) { }

    String runtimePath, tempPath;
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
    PIMPL_DECLARE_P(Context)
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
        Disassembler* disassembler() const;
        void setDisassembler(Disassembler* disassembler);
        void cwd(const String& s);
        void sync(bool b);
        void log(const String& s);
        void problem(const String& s);
        void logproblem(const String& s);
        void status(const String& s);
        void statusProgress(const String& s, size_t progress);
        void statusAddress(const String& s, address_t address);
        bool sync();
        PluginManager* pluginManager() const;
        AbstractUI* ui() const;
        const PluginPaths& pluginPaths() const;
        String capstoneVersion() const;
        String runtimePath() const;
        String tempPath() const;

    public:
        template<typename... T> inline String rnt(const String& p, T... args) const { return Path::create(this->runtimePath(), p, args...); }
        template<typename... T> inline String db(const String& p, T... args) const { return this->rnt("database", p, args...); }
        template<typename... T> inline String signature(const String& p, T... args) const { return this->db("signatures", p, args...); }
};

} // namespace REDasm

#define r_ctx REDasm::Context::instance()
#define r_pm  r_ctx->pluginManager()
#define r_ui  r_ctx->ui()

