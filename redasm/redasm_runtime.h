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
    static LIBREDASM_EXPORT std::string rntDirSeparator;
    static LIBREDASM_EXPORT LogCallback rntLogCallback;
    static LIBREDASM_EXPORT LogCallback rntStatusCallback;
};

} // namespace REDasm

#endif // REDASM_RUNTIME_H
