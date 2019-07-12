#include "demangler.h"
#include <impl/support/demangler_impl.h>
#include "../types/regex.h"

#define MSVC_MANGLED_REGEX   "(\\?.+Z)"

namespace REDasm {

bool Demangler::isMSVC(const String &s, String* result)
{
    RegexMatchList m = Regex(MSVC_MANGLED_REGEX).match(s);

    if(!m.hasMatch())
        return false;

    if(result)
        *result = m.at(1).value;

    return true;
}

bool Demangler::isMangled(const String &s)
{
    if(s.empty())
        return false;

    return Demangler::isMSVC(s) || Demangler::isItanium(s);
}

bool Demangler::isItanium(const String &s, String *result)
{
    if(s.empty() || s.first() != '_')
        return false;

    return (s.indexOf("_Z") == 0) || (s.indexOf("__Z") == 0);
}

String Demangler::demangled(const String &s, bool simplified)
{
    String result;

    if(Demangler::isMSVC(s, &result))
        return DemanglerImpl::demangleMSVC(result, simplified);
    if(Demangler::isItanium(s))
        return DemanglerImpl::demangleItanium(s);

    return s;
}

} // namespace REDasm
