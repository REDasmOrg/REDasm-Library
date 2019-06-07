#pragma once

#include <string>

namespace REDasm {

class DemanglerImpl
{
    public:
        DemanglerImpl() = delete;
        static std::string demangleMSVC(const std::string& s, bool simplified);
        static std::string demangleItanium(const std::string& s);
};

} // namespace REDasm
