#pragma once

#include <string>
#include <cerrno>
#include <redasm/types/string.h>

namespace REDasm {

class StringImpl
{
    PIMPL_DECLARE_Q(String)
    PIMPL_DECLARE_PUBLIC(String)

    public:
        StringImpl() = default;
        String xorified() const;

    public:
        template<typename T, typename Func> T convert(Func func, int base, bool* ok) const;

    private:
        std::string m_data;

     friend class RegexImpl;
     friend class RegexMatchIteratorImpl;
};

template<typename T, typename Func> T StringImpl::convert(Func func, int base, bool *ok) const
{
    char* endptr = nullptr;
    T res = static_cast<T>(func(m_data.c_str(), &endptr, base));

    if(!ok)
        return res;

    *ok = ((*endptr == '\0') || (((res == std::numeric_limits<T>::min()) || (res == std::numeric_limits<T>::max())) && (errno == ERANGE))) ? false : true;
    return res;
}

} // namespace REDasm
