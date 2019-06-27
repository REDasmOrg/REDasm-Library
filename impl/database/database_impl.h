#pragma once

#include <redasm/types/string.h>

namespace REDasm {

class DatabaseImpl
{
    public:
        DatabaseImpl() = delete;

    public:
        static String m_lasterror;
};

} // namespace REDasm
