#include "demangler_impl.h"
#include "../libs/binutils_demangler/demangle.h" // Itanium Demangler
#include "../libs/undname.h"                     // MSVC Demangler
#include <vector>

#define DEMAGLER_BUFFER_SIZE 2048

namespace REDasm {

String DemanglerImpl::demangleMSVC(const String &s, bool simplified)
{
    std::vector<char> v(DEMAGLER_BUFFER_SIZE);
    unsigned short flags = 0;

    if(simplified)
    {
        flags = UNDNAME_NO_MS_KEYWORDS |
                UNDNAME_NO_MEMBER_TYPE |
                UNDNAME_NO_CV_THISTYPE |
                UNDNAME_NO_FUNCTION_RETURNS |
                UNDNAME_NO_ACCESS_SPECIFIERS;
    }

    if(!__unDName(v.data(), s.c_str(), static_cast<int>(v.size()), std::malloc, std::free, flags))
        return s;

    return v.data();
}

String DemanglerImpl::demangleItanium(const String &s)
{
    char* pres = cplus_demangle_v3(s.c_str(), DMGL_NO_OPTS);

    if(!pres)
        return s;

    String res = pres;
    free(pres);
    return res;
}

} // namespace REDasm
