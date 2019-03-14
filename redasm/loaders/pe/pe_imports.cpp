#include "pe_imports.h"

namespace REDasm {

PEImports::ResolveMap PEImports::m_libraries;

std::string PEImports::importModuleName(std::string dllname)
{
    size_t dotidx = dllname.rfind('.');

    if(dotidx != std::string::npos)
        dllname.erase(dotidx);

    return dllname;
}

} // namespace REDasm
