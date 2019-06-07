#include "demangler.h"
#include <impl/support/demangler_impl.h>
#include <regex>

#define MSVC_MANGLED_REGEX   "(\\?.+Z)"

namespace REDasm {

bool Demangler::isMSVC(const std::string &s, std::string* result)
{
    std::smatch match;

    if(!std::regex_search(s, match, std::regex(MSVC_MANGLED_REGEX)))
        return false;

    if(result)
        *result = match[1];

    return true;
}

bool Demangler::isMangled(const std::string &s)
{
    if(s.empty())
        return false;

    return Demangler::isMSVC(s) || Demangler::isItanium(s);
}

bool Demangler::isItanium(const std::string &s, std::string *result)
{
    if(s.empty() || s.front() != '_')
        return false;

    return (s.find("_Z") == 0) || (s.find("__Z") == 0);
}

std::string Demangler::demangled(const std::string &s, bool simplified)
{
    std::string result;

    if(Demangler::isMSVC(s, &result))
        return DemanglerImpl::demangleMSVC(result, simplified);
    if(Demangler::isItanium(s))
        return DemanglerImpl::demangleItanium(s);

    return s;
}

} // namespace REDasm
