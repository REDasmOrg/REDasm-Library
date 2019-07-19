#pragma once

#include <string>
#include <redasm/types/string.h>

namespace REDasm {

class StringImpl
{
    PIMPL_DECLARE_Q(String)
    PIMPL_DECLARE_PUBLIC(String)

    public:
        StringImpl() = default;
        String xorified() const;

    private:
        std::string m_data;

     friend class RegexImpl;
     friend class RegexMatchIteratorImpl;
};

} // namespace REDasm
