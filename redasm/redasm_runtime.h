#ifndef REDASM_RUNTIME_H
#define REDASM_RUNTIME_H

#include <libredasm_export.h>
#include <functional>
#include <string>

namespace REDasm {

struct Runtime
{
    typedef std::function<void(const std::string&)> LogCallback;

    static LIBREDASM_EXPORT std::string rntSearchPath;
    static LIBREDASM_EXPORT std::string rntTempPath;
    static LIBREDASM_EXPORT LogCallback rntLogCallback;
    static LIBREDASM_EXPORT LogCallback rntStatusCallback;

    static const LIBREDASM_EXPORT std::string rntDirSeparator;

    static void cwd(const std::string& s);
    static void sync(bool b);
    static bool sync();
};

} // namespace REDasm

#endif // REDASM_RUNTIME_H
