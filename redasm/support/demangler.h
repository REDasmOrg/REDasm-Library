#pragma once

#include <string>
#include <typeinfo>
#include "../macros.h"

namespace REDasm {

class LIBREDASM_API Demangler
{
    public:
        Demangler() = default;
        static std::string demangled(const std::string& s, bool simplified = true);
        static bool isMSVC(const std::string &s, std::string *result = nullptr);
        static bool isItanium(const std::string &s, std::string *result = nullptr);
        static bool isMangled(const std::string& s);
        template<typename T> static std::string typeName() { return demangled(typeid(T).name()); }
};

} // namespace REDasm
