#pragma once

#include "../types/string.h"
#include <typeinfo>
#include "../macros.h"

namespace REDasm {

class LIBREDASM_API Demangler
{
    public:
        Demangler() = default;
        static String demangled(const String& s, bool simplified = true);
        static bool isMSVC(const String &s, String *result = nullptr);
        static bool isItanium(const String &s, String *result = nullptr);
        static bool isMangled(const String& s);
        template<typename T> static String typeName() { return demangled(typeid(T).name()); }
};

} // namespace REDasm
