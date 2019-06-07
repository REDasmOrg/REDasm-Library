#pragma once

#include <string>

namespace REDasm {

class DatabaseImpl
{
    public:
        DatabaseImpl() = delete;

    public:
        static std::string m_lasterror;
};

} // namespace REDasm
