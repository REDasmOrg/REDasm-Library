#include "redasm_runtime.h"
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

std::string Runtime::rntSearchPath;
std::string Runtime::rntTempPath;
std::chrono::steady_clock::time_point Runtime::rntLastStatusReport = std::chrono::steady_clock::now();
const std::chrono::milliseconds Runtime::rntDebounceTimeout = std::chrono::milliseconds(100);

#ifdef _WIN32
const std::string Runtime::rntDirSeparator = "\\";
#else
const std::string Runtime::rntDirSeparator = "/";
#endif

Runtime::LogCallback Runtime::rntLogCallback = [](const std::string& s) { std::cout << s << std::endl; };
Runtime::LogCallback Runtime::rntStatusCallback = [](const std::string&) { };
Runtime::ProgressCallback Runtime::rntProgressCallback = [](size_t) { };

void Runtime::cwd(const std::string &s)
{
#ifdef _WIN32
    SetCurrentDirectory(s.c_str());
#elif __unix__
    chdir(s.c_str());
#else
    #error "cwd: Unsupported Platform"
#endif
}

void Runtime::sync(bool b)
{
    if(b)
        putenv(const_cast<char*>("SYNC_MODE=1"));
    else
        putenv(const_cast<char*>("SYNC_MODE=0"));
}

bool Runtime::sync()
{
    const char* syncmode = getenv("SYNC_MODE");
    return syncmode && !std::strcmp(syncmode, "1");
}

} // namespace REDasm
