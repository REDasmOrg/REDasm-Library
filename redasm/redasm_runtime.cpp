#include "redasm_runtime.h"
#include <iostream>

namespace REDasm {

std::string Runtime::rntSearchPath;

#ifdef _WIN32
std::string Runtime::rntDirSeparator = "\\";
#else
std::string Runtime::rntDirSeparator = "/";
#endif

Runtime::LogCallback Runtime::rntLogCallback = [](const std::string& s) { std::cout << s << std::endl; };
Runtime::LogCallback Runtime::rntStatusCallback = [](const std::string&) { };

} // namespace REDasm
