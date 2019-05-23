#include "redasm_context.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>

#ifdef _WIN32
    #include <windows.h>
    #include <winbase.h>
#else
    #include <unistd.h>
#endif

namespace REDasm {

ContextSettings Context::settings;
std::set<std::string> Context::m_uproblems;
ProblemList Context::m_problems;
std::chrono::steady_clock::time_point Context::lastStatusReport = std::chrono::steady_clock::now();
const std::chrono::milliseconds Context::debounceTimeout = std::chrono::milliseconds(100);

#ifdef _WIN32
const std::string Context::dirSeparator = "\\";
#else
const std::string Context::dirSeparator = "/";
#endif

bool Context::hasProblems() { return !m_problems.empty(); }
size_t Context::problemsCount() { return m_problems.size(); }
const ProblemList &Context::problems() { return m_problems; }

void Context::problem(const std::string &s)
{
    if(Context::settings.ignoreproblems)
        return;

    auto it = m_uproblems.find(s);

    if(it != m_uproblems.end())
        return;

    m_uproblems.insert(s);
    m_problems.push_back(s);
}

void Context::clearProblems()
{
    m_uproblems.clear();
    m_problems.clear();
}

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

void Context::sync(bool b)
{
    if(b)
        putenv(const_cast<char*>("SYNC_MODE=1"));
    else
        putenv(const_cast<char*>("SYNC_MODE=0"));
}

bool Context::sync()
{
    const char* syncmode = getenv("SYNC_MODE");
    return syncmode && !std::strcmp(syncmode, "1");
}

} // namespace REDasm
