#include "redasm_runtime.h"
#include <iostream>
#include <cstdlib>
#include <cstring>

namespace REDasm {

std::string Runtime::rntSearchPath;
std::string Runtime::rntTempPath;

#ifdef _WIN32
const std::string Runtime::rntDirSeparator = "\\";
#else
const std::string Runtime::rntDirSeparator = "/";
#endif

Runtime::LogCallback Runtime::rntLogCallback = [](const std::string& s) { std::cout << s << std::endl; };
Runtime::LogCallback Runtime::rntStatusCallback = [](const std::string&) { };

bool Runtime::syncMode()
{
    const char* syncmode = getenv("SYNC_MODE");
    return syncmode && !std::strcmp(syncmode, "1");
}

void Runtime::syncMode(bool b)
{
    if(b)
        putenv(const_cast<char*>("SYNC_MODE=1"));
    else
        putenv(const_cast<char*>("SYNC_MODE=0"));
}

} // namespace REDasm
