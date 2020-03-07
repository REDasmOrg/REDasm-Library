#pragma once

#include <functional>
#include <list>
#include "support/event/eventmanager.h"
#include "plugins/pluginmanager.h"
#include "plugins/loader/loader.h"
#include "types/container.h"
#include "support/filesystem.h"
#include "pimpl.h"
#include "ui.h"

namespace REDasm {

typedef std::function<void(const String&)> Context_LogCallback;
typedef std::function<void(size_t)> Context_ProgressCallback;
typedef std::deque<String> ProblemList;
typedef ListAdapter<String> PluginPaths;

struct LIBREDASM_API ContextSettings
{
    ContextSettings(): ignoreproblems(false) { }

    String runtimePath, tempPath;
    Context_LogCallback logCallback;
    Context_LogCallback statusCallback;
    Context_ProgressCallback progressCallback;
    std::shared_ptr<AbstractUI> ui;
    bool ignoreproblems;
};

class ContextImpl;

class LIBREDASM_API Context
{
    PIMPL_DECLARE_P(Context)
    PIMPL_DECLARE_PRIVATE(Context)

    public:
        enum Flags: flag_t
        {
            None              = (1 << 0),
            DisableUnexplored = (1 << 1),
            DisableAnalyzer   = (1 << 2),
            DisableSignature  = (1 << 3),
            DisableCFG        = (1 << 4),
        };

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
        void addPluginPath(const String& s);
        void cwd(const String& s);
        void sync(bool b);
        void log(const String& s);
        void problem(const String& s);
        void logproblem(const String& s);
        void status(const String& s);
        void statusProgress(const String& s, size_t progress);
        void statusAddress(const String& s, address_t address);
        bool sync();
        bool hasFlag(flag_t flag) const;
        void flag(flag_t flag, bool set = true);
        void flags(flag_t flags);
        PluginManager* pluginManager() const;
        AbstractUI* ui() const;
        PluginPaths* pluginPaths() const;
        String capstoneVersion() const;
        String runtimePath() const;
        String tempPath() const;

    public:
        String rnt(const FS::Path& p) const;
        String db(const FS::Path& p) const;
        String signaturedb(const FS::Path& p) const;
        String loaderdb(const FS::Path& p) const;
        String assemblerdb(const FS::Path& p) const;
        String plugindb(const FS::Path& p) const;
};

} // namespace REDasm

#define r_evt    REDasm::EventManager
#define r_ctx    REDasm::Context::instance()
#define r_pm     r_ctx->pluginManager()
#define r_ui     r_ctx->ui()
#define r_disasm r_ctx->disassembler()
#define r_doc    r_disasm->document()
#define r_ldr    r_disasm->loader()
#define r_asm    r_disasm->assembler()
