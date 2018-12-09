#include "pe_imports.h"

namespace REDasm {

PEImports::ResolveMap PEImports::m_libraries;

PEImports::PEImports()
{

}

void PEImports::loadImport(const std::string &dllname)
{
    if(m_libraries.find(dllname) != m_libraries.end())
        return;

    m_libraries[dllname] = OrdinalsMap();

    REDasm::loadordinals(REDasm::makeFormatPath("pe",  PEImports::importModuleName(dllname) + ".json"),
                         m_libraries[dllname]);
}

std::string PEImports::importModuleName(std::string dllname)
{
    size_t dotidx = dllname.rfind('.');

    if(dotidx != std::string::npos)
        dllname.erase(dotidx);

    return dllname;
}

bool PEImports::importName(const std::string &dllname, u16 ordinal, std::string &name)
{
    PEImports::loadImport(dllname);

    auto it = m_libraries.find(dllname);

    if(it == m_libraries.end())
        return false;

    name = REDasm::ordinal(it->second, ordinal);
    return true;
}

} // namespace REDasm
