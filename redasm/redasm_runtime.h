#ifndef REDASM_RUNTIME_H
#define REDASM_RUNTIME_H

#include <libredasm_export.h>
#include <functional>
#include <chrono>
#include <string>

#define RUNTIME_DEBOUNCE_CHECK  auto now = std::chrono::steady_clock::now(); \
                                if((now - Runtime::rntLastStatusReport) < Runtime::rntDebounceTimeout) return; \
                                Runtime::rntLastStatusReport = now;

namespace REDasm {

struct Runtime
{
    typedef std::function<void(const std::string&)> LogCallback;
    typedef std::function<void(size_t)> ProgressCallback;

    static LIBREDASM_EXPORT std::string rntSearchPath;
    static LIBREDASM_EXPORT std::string rntTempPath;
    static LIBREDASM_EXPORT LogCallback rntLogCallback;
    static LIBREDASM_EXPORT LogCallback rntStatusCallback;
    static LIBREDASM_EXPORT ProgressCallback rntProgressCallback;
    static LIBREDASM_EXPORT std::chrono::steady_clock::time_point rntLastStatusReport;

    static const LIBREDASM_EXPORT std::string rntDirSeparator;
    static const LIBREDASM_EXPORT std::chrono::milliseconds rntDebounceTimeout;

    static void cwd(const std::string& s);
    static void sync(bool b);
    static bool sync();
};

} // namespace REDasm

#endif // REDASM_RUNTIME_H
