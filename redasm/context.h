#pragma once

#include <functional>
#include <list>
#include "support/event/eventmanager.h"
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

enum class ContextFlags
{
    None            = 0,
    StepDisassembly = (1 << 0)
};

ENUM_FLAGS_OPERATORS(ContextFlags)

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
        static void inherit(Context *ctx);
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
        bool hasFlag(ContextFlags flag) const;
        void flag(ContextFlags flag, bool set = true);
        PluginManager* pluginManager() const;
        AbstractUI* ui() const;
        const PluginPaths& pluginPaths() const;
        String capstoneVersion() const;
        String runtimePath() const;
        String tempPath() const;

    public:
        template<typename... T> inline String rnt(const String& p, T... args) const { return Path::create(this->runtimePath(), p, args...); }
        template<typename... T> inline String db(const String& p, T... args) const { return this->rnt("database", p, args...); }
        template<typename... T> inline String signaturedb(const String& p, T... args) const { return this->db("signatures", p, args...); }
        template<typename... T> inline String loaderdb(const String& p, T... args) const { return this->db("loaders", p, args...); }
        template<typename... T> inline String assemblerdb(const String& p, T... args) const { return this->db("assemblers", p, args...); }
        template<typename... T> inline String plugindb(const String& p, T... args) const { return this->db("plugins", p, args...); }
};

} // namespace REDasm

#define r_evt    REDasm::EventManager
#define r_ctx    REDasm::Context::instance()
#define r_pm     r_ctx->pluginManager()
#define r_ui     r_ctx->ui()
#define r_disasm r_ctx->disassembler()
#define r_doc    r_disasm->document()
#define r_docnew r_disasm->documentNew()
#define r_ldr    r_disasm->loader()
#define r_asm    r_disasm->assembler()
