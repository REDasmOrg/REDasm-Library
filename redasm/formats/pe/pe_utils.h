#ifndef PE_UTILS_H
#define PE_UTILS_H

#include <string>
#include "../../redasm.h"

namespace REDasm {

class PEUtils
{
    public:
        PEUtils() = delete;
        static std::string importName(std::string library, const std::string& name);
        static std::string importName(std::string library, s64 ordinal);
        static bool checkMsvcImport(const std::string& importdescriptor);
};

}

#endif // PE_UTILS_H
