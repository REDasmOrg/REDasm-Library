#include "context.h"
#include "support/utils.h"
#include "disassembler/disassembler.h"
#include <impl/types/container_impl.h>
#include <impl/context_impl.h>

#define CONTEXT_DEBOUNCE_CHECK  auto now = std::chrono::steady_clock::now(); \
                                if((now - pimpl_p()->m_laststatusreport) < pimpl_p()->m_debouncetimeout) return; \
                                pimpl_p()->m_laststatusreport = now;

#ifdef _WIN32
    #include <windows.h>
    #include <winbase.h>
#else
    #include <unistd.h>
#endif

namespace REDasm {

Context::Context(): m_pimpl_p(new ContextImpl()) { }
void Context::inherit(Context *ctx) { ContextImpl::m_parentinstance = ctx; }

Context *Context::init(const ContextSettings &settings)
{
    if(ContextImpl::m_instance)
        return ContextImpl::m_instance.get();

    ContextImpl::m_instance = std::unique_ptr<Context>(new Context());
    ContextImpl::m_instance->pimpl_p()->m_settings = settings;
    ContextImpl::m_instance->pimpl_p()->checkSettings();
    return ContextImpl::m_instance.get();
}

Context *Context::instance() { return ContextImpl::m_parentinstance ? ContextImpl::m_parentinstance : ContextImpl::m_instance.get(); }
bool Context::hasProblems() const { PIMPL_P(const Context); return !p->m_problems.empty(); }
size_t Context::problemsCount() const { PIMPL_P(const Context); return p->m_problems.size(); }
const ProblemList &Context::problems() const { PIMPL_P(const Context); return p->m_problems; }
void Context::clearProblems() { PIMPL_P(Context); p->m_uproblems.clear(); p->m_problems.clear(); }
Disassembler *Context::disassembler() const { PIMPL_P(const Context); return p->disassembler(); }
void Context::setDisassembler(Disassembler *disassembler) { PIMPL_P(Context); p->setDisassembler(disassembler); }
void Context::addPluginPath(const String& s) { PIMPL_P(Context); p->addPluginPath(s); }

void Context::cwd(const String &s)
{
#ifdef _WIN32
    SetCurrentDirectory(s.c_str());
#elif defined(__unix__) || defined(__APPLE__)
    chdir(s.c_str());
#else
#error "cwd: Unsupported Platform"
#endif
}

void Context::sync(bool b) { putenv(const_cast<char*>(b ? "SYNC_MODE=1" : "SYNC_MODE=0")); }

bool Context::sync()
{
    const char* syncmode = getenv("SYNC_MODE");
    return syncmode && !std::strcmp(syncmode, "1");
}

bool Context::hasFlag(flag_t flag) const { PIMPL_P(const Context); return p->hasFlag(flag); }
void Context::flag(flag_t flag, bool set) { PIMPL_P(Context); p->flag(flag, set); }
void Context::flags(flag_t flags) { PIMPL_P(Context); p->flags(flags); }
PluginManager *Context::pluginManager() const { return PluginManager::instance(); }
AbstractUI *Context::ui() const { PIMPL_P(const Context); return p->m_settings.ui.get(); }
PluginPaths* Context::pluginPaths() const { PIMPL_P(const Context); return REDasm::list_adapter<String>(&p->m_pluginpaths); }
String Context::runtimePath() const { PIMPL_P(const Context); return p->m_settings.runtimePath; }
String Context::tempPath() const { PIMPL_P(const Context); return p->m_settings.tempPath;  }
String Context::rnt(const FS::Path& p) const { return FS::Path::join(this->runtimePath(), p.value); }
String Context::db(const FS::Path& p) const { return this->rnt(FS::Path::join("database", p.value)); }
String Context::signaturedb(const FS::Path& p) const { return this->db(FS::Path::join("signatures", p.value)); }
String Context::loaderdb(const FS::Path& p) const { return this->db(FS::Path::join("loaders", p.value)); }
String Context::assemblerdb(const FS::Path& p) const { return this->db(FS::Path::join("assemblers", p.value)); }
String Context::plugindb(const FS::Path& p) const { return this->db(FS::Path::join("plugins", p.value)); }

String Context::capstoneVersion() const { PIMPL_P(const Context); return p->capstoneVersion(); }
void Context::log(const String &s) { PIMPL_P(Context); p->m_settings.logCallback(s); }

void Context::problem(const String &s)
{
    PIMPL_P(Context);

    if(p->m_settings.ignoreproblems)
        return;

    std::lock_guard<std::mutex> lock(p->m_mutex);

    auto it = p->m_uproblems.find(s);

    if(it != p->m_uproblems.end())
        return;

    p->m_uproblems.insert(s);
    p->m_problems.push_back(s);
}

void Context::logproblem(const String &s) { this->log(s); this->problem(s); }

void Context::status(const String &s)
{
    PIMPL_P(Context);
    ContextImpl::log_lock lock(p->m_mutex);

    CONTEXT_DEBOUNCE_CHECK
    p->m_settings.statusCallback(s);
}

void Context::statusProgress(const String &s, size_t progress)
{
    PIMPL_P(Context);
    ContextImpl::log_lock lock(p->m_mutex);

    if(progress) {
        CONTEXT_DEBOUNCE_CHECK
    }

    p->m_settings.statusCallback(s);
    p->m_settings.progressCallback(progress);
}

void Context::statusAddress(const String &s, address_t address)
{
    PIMPL_P(Context);
    ContextImpl::log_lock lock(p->m_mutex);

    CONTEXT_DEBOUNCE_CHECK
    p->m_settings.statusCallback(s + " @ " + String::hex(address));
}

} // namespace REDasm
