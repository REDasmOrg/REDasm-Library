#pragma once

#include <string>

class Demangler
{
    public:
        Demangler() = delete;
        static const char* demangled(const std::string& s, bool simplified = true);
        static bool isMSVC(const std::string& s, std::string* result = nullptr);
        static bool isItanium(const std::string& s);
        static bool isMangled(const std::string& s);

    private:
        static std::string demangleMSVC(const std::string& s, bool simplified);
        static std::string demangleItanium(const std::string& s);
};
