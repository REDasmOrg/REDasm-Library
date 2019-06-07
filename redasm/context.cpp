#include "context.h"
#include "support/utils.h"
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

std::unique_ptr<Context> Context::m_instance;
Context* Context::m_parentinstance = nullptr;

Context::Context(): m_pimpl_p(new ContextImpl()) { }
void Context::init(Context *ctx) { m_parentinstance = ctx; }

Context *Context::init(const ContextSettings &settings)
{
    if(m_instance)
        return m_instance.get();

    m_instance = std::unique_ptr<Context>(new Context());
    m_instance->pimpl_p()->m_settings = settings;
    m_instance->pimpl_p()->checkSettings();
    return m_instance.get();
}

Context *Context::instance() { return m_parentinstance ? m_parentinstance : m_instance.get(); }
bool Context::hasProblems() const { return !pimpl_p()->m_problems.empty(); }
size_t Context::problemsCount() const { return pimpl_p()->m_problems.size(); }
const ProblemList &Context::problems() const { return pimpl_p()->m_problems; }
void Context::clearProblems() { pimpl_p()->m_uproblems.clear(); pimpl_p()->m_problems.clear(); }

void Context::cwd(const std::string &s)
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

PluginManager *Context::pluginManager() const { return PluginManager::instance(); }
std::string Context::capstoneVersion() const { PIMPL_P(const Context); return p->capstoneVersion(); }
void Context::log(const std::string &s) { PIMPL_P(Context); p->m_settings.logCallback(s); }

void Context::problem(const std::string &s)
{
    PIMPL_P(Context);

    if(p->m_settings.ignoreproblems)
        return;

    auto it = p->m_uproblems.find(s);

    if(it != p->m_uproblems.end())
        return;

    p->m_uproblems.insert(s);
    p->m_problems.push_back(s);
}

void Context::logproblem(const std::string &s) { this->log(s); this->problem(s); }

void Context::status(const std::string &s)
{
    CONTEXT_DEBOUNCE_CHECK
    PIMPL_P(Context);
    p->m_settings.statusCallback(s);
}

void Context::statusProgress(const std::string &s, size_t progress)
{
    CONTEXT_DEBOUNCE_CHECK
    PIMPL_P(Context);
    p->m_settings.statusCallback(s);
    p->m_settings.progressCallback(progress);
}

void Context::statusAddress(const std::string &s, address_t address)
{
    CONTEXT_DEBOUNCE_CHECK
    PIMPL_P(Context);
    p->m_settings.statusCallback(s + " @ " + Utils::hex(address));
}

std::string Context::runtimePath() const { PIMPL_P(const Context); return p->m_settings.runtimePath; }
std::string Context::pluginPath() const { PIMPL_P(const Context); return p->m_settings.pluginPath; }
std::string Context::tempPath() const { PIMPL_P(const Context); return p->m_settings.tempPath;  }

} // namespace REDasm
