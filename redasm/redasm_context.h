#pragma once

#include <libredasm_export.h>
#include <functional>
#include <memory>
#include <deque>
#include <set>
#include <chrono>
#include <string>
#include "redasm_ui.h"

#define CONTEXT_DEBOUNCE_CHECK  auto now = std::chrono::steady_clock::now(); \
                                if((now - Context::lastStatusReport) < Context::debounceTimeout) return; \
                                Context::lastStatusReport = now;

namespace REDasm {

typedef std::function<void(const std::string&)> Context_LogCallback;
typedef std::function<void(size_t)> Context_ProgressCallback;
typedef std::deque<std::string> ProblemList;

struct ContextSettings
{
    ContextSettings(): ignoreproblems(false) { }

    std::string searchPath;
    std::string tempPath;
    Context_LogCallback logCallback;
    Context_LogCallback statusCallback;
    Context_ProgressCallback progressCallback;
    std::shared_ptr<AbstractUI> ui;
    bool ignoreproblems;
};

struct Context
{
    static LIBREDASM_EXPORT ContextSettings settings;
    static LIBREDASM_EXPORT std::chrono::steady_clock::time_point lastStatusReport;

    static const LIBREDASM_EXPORT std::string dirSeparator;
    static const LIBREDASM_EXPORT std::chrono::milliseconds debounceTimeout;

    static bool hasProblems();
    static size_t problemsCount();
    static const ProblemList& problems();
    static void problem(const std::string& s);
    static void clearProblems();
    static void cwd(const std::string& s);
    static void sync(bool b);
    static bool sync();

    private:
        static std::set<std::string> m_uproblems;
        static ProblemList m_problems;
};

#define r_ui Context::settings.ui

} // namespace REDasm
