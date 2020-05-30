#include "demangler.h"
#include <regex>
#include <algorithm>
#include <cstring>
#include "../libs/binutils_demangler/demangle.h" // Itanium Demangler
#include "../libs/undname.h"                     // MSVC Demangler

#define DEMAGLER_BUFFER_SIZE 2048
#define MSVC_MANGLED_REGEX   "(\\?.+Z)"

const char* Demangler::demangled(const std::string& s, bool simplified)
{
    static std::string result;
    result = s;

    if(Demangler::isMSVC(s, &result)) result = Demangler::demangleMSVC(result, simplified);
    else if(Demangler::isItanium(s)) result = Demangler::demangleItanium(s);
    return result.c_str();
}

bool Demangler::isMSVC(const std::string& s, std::string* result)
{
    static std::regex rgx(MSVC_MANGLED_REGEX);

    std::smatch sm;
    if(!std::regex_match(s,sm, rgx)) return false;
    *result = sm.str(1);
    return true;
}

bool Demangler::isItanium(const std::string& s)
{
    if(s.empty() || s[0] != '_') return false;

    return (s.find_first_of("_Z") != std::string::npos) ||
           (s.find_first_of("__Z") != std::string::npos);
}

bool Demangler::isMangled(const std::string& s)
{
    if(s.empty()) return false;
    return Demangler::isMSVC(s) || Demangler::isItanium(s);
}

std::string Demangler::demangleMSVC(const std::string& s, bool simplified)
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

std::string Demangler::demangleItanium(const std::string& s)
{
    char* pres = cplus_demangle_v3(s.c_str(), DMGL_NO_OPTS);
    if(!pres) return s;

    std::string res = pres;
    free(pres);
    return res;
}
