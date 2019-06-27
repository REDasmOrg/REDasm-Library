#pragma once

#include <redasm/types/string.h>

namespace REDasm {

class DemanglerImpl
{
    public:
        DemanglerImpl() = delete;
        static String demangleMSVC(const String& s, bool simplified);
        static String demangleItanium(const String& s);
};

} // namespace REDasm
