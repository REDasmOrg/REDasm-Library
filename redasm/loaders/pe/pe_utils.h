#pragma once

#include <string>
#include "../../redasm.h"
#include "pe_header.h"

namespace REDasm {

class PEUtils
{
    public:
        PEUtils() = delete;
        static std::string sectionName(const char* psectionname);
        static std::string importName(std::string library, const std::string& name);
        static std::string importName(const std::string &library, s64 ordinal);
        static bool checkMsvcImport(const std::string& importdescriptor);
        static offset_location rvaToOffset(const ImageNtHeaders* ntheaders, u64 rva);
};

} // namespace REDasm
