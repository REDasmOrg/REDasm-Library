#ifndef REDASM_CONTEXT_H
#define REDASM_CONTEXT_H

#include <libredasm_export.h>
#include <functional>
#include <chrono>
#include <string>

#define CONTEXT_DEBOUNCE_CHECK  auto now = std::chrono::steady_clock::now(); \
                                if((now - Context::lastStatusReport) < Context::debounceTimeout) return; \
                                Context::lastStatusReport = now;

namespace REDasm {

typedef std::function<void(const std::string&)> Context_LogCallback;
typedef std::function<void(size_t)> Context_ProgressCallback;

struct ContextSettings
{
    std::string searchPath;
    std::string tempPath;
    Context_LogCallback logCallback;
    Context_LogCallback statusCallback;
    Context_ProgressCallback progressCallback;
};

struct Context
{

    static LIBREDASM_EXPORT ContextSettings settings;
    static LIBREDASM_EXPORT std::chrono::steady_clock::time_point lastStatusReport;

    static const LIBREDASM_EXPORT std::string dirSeparator;
    static const LIBREDASM_EXPORT std::chrono::milliseconds debounceTimeout;

    static void cwd(const std::string& s);
    static void sync(bool b);
    static bool sync();
};

} // namespace REDasm

#endif // REDASM_CONTEXT_H
