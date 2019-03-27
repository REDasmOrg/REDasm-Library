#include "pe_utils.h"
#include "pe_constants.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace REDasm {

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

    if(!REDasm::endsWith(library, ".dll"))
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

offset_location PEUtils::rvaToOffset(const ImageNtHeaders *ntheaders, u64 rva)
{
    const ImageSectionHeader* sectiontable = IMAGE_FIRST_SECTION(ntheaders);

    for(size_t i = 0; i < ntheaders->FileHeader.NumberOfSections; i++)
    {
        const ImageSectionHeader& section = sectiontable[i];

        if((rva >= section.VirtualAddress) && (rva < (section.VirtualAddress + section.Misc.VirtualSize)))
        {
            if(!section.SizeOfRawData) // Check if section not BSS
                break;

            offset_t offset = section.PointerToRawData + (rva - section.VirtualAddress);
            return REDasm::make_location(offset, offset < (section.PointerToRawData + section.SizeOfRawData));
        }
    }

    return REDasm::invalid_location<offset_t>();
}

} // namespace REDasm
