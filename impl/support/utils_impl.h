#pragma once

#define R2(n)     n,     n + 2*64,     n + 1*64,     n + 3*64
#define R4(n) R2(n), R2(n + 2*16), R2(n + 1*16), R2(n + 3*16)
#define R6(n) R4(n), R4(n + 2*4 ), R4(n + 1*4 ), R4(n + 3*4 )
#define REVERSE_BITS R6(0), R6(2), R6(1), R6(3)

#include <redasm/types/string.h>
#include <climits>

namespace REDasm {

class UtilsImpl
{
    public:
        UtilsImpl() = delete;
        template<typename Container> static inline String join(const Container& c, const char* sep);
        template<typename T> static T bitreverse(T val);

    private:
        static const unsigned int RBITS_TABLE[256];
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

template<typename T> T UtilsImpl::bitreverse(T val)
{
    unsigned int p = (sizeof(T) * CHAR_BIT) - CHAR_BIT;
    T rval = 0;

    for(unsigned int i = 0; i < sizeof(T); i++, p -= CHAR_BIT)
        rval |= UtilsImpl::RBITS_TABLE[(val >> (i * CHAR_BIT)) & 0xFF] << p;

    return rval;
}

} // namespace REDasm
