#include "pe_utils.h"
#include "pe_constants.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace REDasm {

static bool endsWith(const std::string &str, const std::string &suffix)
{
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string PEUtils::sectionName(const char *psectionname)
{
    const char *pend = psectionname;

    for(size_t i = 0; i < IMAGE_SIZEOF_SHORT_NAME; i++, pend++)
    {
        if(!*pend)
            break;
    }

    return std::string(psectionname, pend);
}

std::string PEUtils::importName(std::string library, const std::string &name)
{
    std::transform(library.begin(), library.end(), library.begin(), ::tolower);

    if(!endsWith(library, ".dll"))
        library += ".dll";

    std::stringstream ss;
    ss << library << "_" << name;
    return ss.str();
}

std::string PEUtils::importName(const std::string& library, s64 ordinal)
{
    std::stringstream ss;
    ss << "Ordinal__" << std::uppercase << std::setw(4) << std::setfill('0') << std::setbase(16) << ordinal;
    return PEUtils::importName(library, ss.str());
}

bool PEUtils::checkMsvcImport(const std::string &importdescriptor)
{
    if(!importdescriptor.find("vcruntime"))
        return true;
    if(!importdescriptor.find("mfc"))
        return true;
    if(!importdescriptor.find("api-ms-win-crt-"))
        return true;

    return false;
}

} // namespace REDasm
