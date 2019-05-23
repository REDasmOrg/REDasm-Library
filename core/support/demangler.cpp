#include "demangler.h"
#include <undname.h>  // MSVC Demangler
#include <demangle.h> // Itanium Demangler
#include <vector>
#include <regex>

#define DEMAGLER_BUFFER_SIZE 2048
#define MSVC_MANGLED_REGEX   "(\\?.+Z)"

namespace REDasm {
namespace Demangler {

std::string demangleMSVC(const std::string& s, bool simplified) {
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

std::string demangleItanium(const std::string& s) {
    char* pres = cplus_demangle_v3(s.c_str(), DMGL_NO_OPTS);

    if(!pres)
        return s;

    std::string res = pres;
    free(pres);
    return res;
}

bool isMSVC(const std::string &s, std::string* result) {
    std::smatch match;

    if(!std::regex_search(s, match, std::regex(MSVC_MANGLED_REGEX)))
        return false;

    if(result)
        *result = match[1];

    return true;
}

bool isMangled(const std::string &s) {
    if(s.empty())
        return false;

    return Demangler::isMSVC(s) || Demangler::isItanium(s);
}

bool isItanium(const std::string &s, std::string *result)
{
    if(s.empty() || s.front() != '_')
        return false;

    return (s.find("_Z") == 0) || (s.find("__Z") == 0);
}

std::string demangled(const std::string &s, bool simplified) {
    std::string result;

    if(Demangler::isMSVC(s, &result))
        return demangleMSVC(result, simplified);
    if(Demangler::isItanium(s))
        return demangleItanium(s);

    return s;
}

} // namespace Demangler
} // namespace REDasm
