#pragma once

#include <algorithm>
#include <type_traits>
#include <sstream>
#include <iomanip>
#include <climits>
#include <cctype>
#include <cmath>
#include <cassert>
#include "../buffer/bufferview.h"

namespace REDasm {

template<typename T> struct bits_count { static constexpr size_t value = sizeof(T) * CHAR_BIT; };

class Utils
{
    public:
        Utils() = delete;
        static inline String trampoline(const String& s, const String& prefix = String()) { return prefix + "_" + s; }
        static inline bool byte(const String& s, u8* val, size_t offset = 0);

    public:
        template<typename T, typename U> static inline T readpointer(U** p) { T v = *reinterpret_cast<T*>(*p); *p = Utils::relpointer<U>(*p, sizeof(T)); return v; }
        template<typename T, typename ST = typename signed_of<T>::type> static inline ST twoc(T val) { return static_cast<ST>((~val) + 1); }
        template<typename T, typename U> static inline T rol(T n, U c) { assert(c < bits_count<T>::value);  return !c ? n : ((n << c) | (n >> (bits_count<T>::value - c))); }
        template<typename T, typename U> static inline T ror(T n, U c) { assert(c < bits_count<T>::value); return !c ? n : ((n >> c) | (n << (bits_count<T>::value - c))); }
        template<typename T, typename U> static inline T aligned(T t, U a) { T r = t % a; return r ? (t + (a - r)) : t; }
        template<typename T> static inline size_t countbits(T val) { double bytes = std::log(static_cast<double>(val)) / std::log(256.0); return static_cast<size_t>(std::ceil(bytes)) * 8; }
        template<typename T> static inline String dec(T t) { std::stringstream ss; ss << t; return ss.str().c_str(); }
        template<typename T> static inline T unmask(T val, T mask);

    public:
        template<typename T, typename U> static inline T* relpointer(U* base, size_t offset) { return reinterpret_cast<T*>(reinterpret_cast<size_t>(base) + offset); }

    public:
        template<typename Container> static inline String join(const Container& c, const char* sep);
        template<typename T> static inline size_t countbits_r(T val);
};

bool Utils::byte(const String &s, u8 *val, size_t offset)
{
    if(offset > (s.size() - 2))
        return false;

    if(!std::isxdigit(s[offset]) || !std::isxdigit(s[offset + 1]))
        return false;

    *val = static_cast<u8>(s.substring(offset, 2).toInt(16));
    return true;
}

template<typename Container> String Utils::join(const Container& c, const char* sep)
{
    String res;

    for(auto it = c.begin(); it != c.end(); it++) {
        if(it != c.begin())
            res += sep;

        res += *it;
    }

    return res;
}

template<typename T> T Utils::unmask(T val, T mask)
{
    T result = 0;

    for(T i = 0, j = 0; i < bits_count<T>::value; i++) {
        if(!(mask & (1 << i))) // Check if mask bit is set
            continue;

        if(val & (1 << i)) // Check if data bit is set
            result |= (1 << j);

        j++;
    }

    return result;
}

template<typename T> size_t Utils::countbits_r(T val)
{
    size_t bits = countbits(val);

    if(bits <= 8) return 8;
    if(bits <= 16) return 16;
    if(bits <= 64) return 64;

    throw std::runtime_error("Bits out of range: " + std::to_string(bits));
}

} // namespace REDasm
