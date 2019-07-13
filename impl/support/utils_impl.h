#pragma once

#include <redasm/types/string.h>

namespace REDasm {

class UtilsImpl
{
    public:
        UtilsImpl() = delete;
        template<typename Container> static inline String join(const Container& c, const char* sep);
};

template<typename Container> String UtilsImpl::join(const Container& c, const char* sep)
{
    String res;

    for(auto it = c.begin(); it != c.end(); it++) {
        if(it != c.begin())
            res += sep;

        res += *it;
    }

    return res;
}

} // namespace REDasm
