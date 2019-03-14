#ifndef PE_IMPORTS_H
#define PE_IMPORTS_H

#include <unordered_map>
#include <string>
#include "../../redasm.h"
#include "../../support/ordinals.h"

namespace REDasm {

class PEImports
{
    private:
        typedef std::map<std::string, OrdinalsMap> ResolveMap;

    private:
        template<int b> static void loadImport(const std::string &dllname);
        template<int b> static void checkX64(std::string& modulename);

    public:
        PEImports() = delete;
        static std::string importModuleName(std::string dllname);
        template<int b> static bool importName(const std::string& dllname, u16 ordinal, std::string& name);

    private:
        static ResolveMap m_libraries;
};

template<int b> void PEImports::checkX64(std::string &modulename)
{
    if((b != 64) || modulename.find("mfc"))
        return;

    modulename += "!x64";
}

template<int b> void PEImports::loadImport(const std::string& dllname)
{
    std::string modulename = PEImports::importModuleName(dllname);
    PEImports::checkX64<b>(modulename);

    if(m_libraries.find(modulename) != m_libraries.end())
        return;

    m_libraries[modulename] = OrdinalsMap();

    REDasm::loadordinals(REDasm::makeLoaderPath(REDasm::makePath("pe", "ordinals"), modulename + ".json"),
                         m_libraries[dllname]);
}

template<int b> bool PEImports::importName(const std::string &dllname, u16 ordinal, std::string &name)
{
    PEImports::loadImport<b>(dllname);

    auto it = m_libraries.find(dllname);

    if(it == m_libraries.end())
        return false;

    name = REDasm::ordinal(it->second, ordinal);
    return true;
}

} // namespace REDasm

#endif // PE_IMPORTS_H
