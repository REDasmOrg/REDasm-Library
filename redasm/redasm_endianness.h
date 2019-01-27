#ifndef REDASM_ENDIANNESS_H
#define REDASM_ENDIANNESS_H

#include <algorithm>
#include <type_traits>
#include "redasm_types.h"

#define REDASM_ENDIAN_TYPES(name, base, suffix) template<> struct name<s16##suffix>: public base { }; \
                                                template<> struct name<s32##suffix>: public base { }; \
                                                template<> struct name<s64##suffix>: public base { }; \
                                                template<> struct name<u16##suffix>: public base { }; \
                                                template<> struct name<u32##suffix>: public base { }; \
                                                template<> struct name<u64##suffix>: public base { };

#define REDASM_ENDIAN_TYPES_COND(name, cond) template<> struct name<s16>: public std::conditional<cond, std::true_type, std::false_type>::type { }; \
                                             template<> struct name<s32>: public std::conditional<cond, std::true_type, std::false_type>::type { }; \
                                             template<> struct name<s64>: public std::conditional<cond, std::true_type, std::false_type>::type { }; \
                                             template<> struct name<u16>: public std::conditional<cond, std::true_type, std::false_type>::type { }; \
                                             template<> struct name<u32>: public std::conditional<cond, std::true_type, std::false_type>::type { }; \
                                             template<> struct name<u64>: public std::conditional<cond, std::true_type, std::false_type>::type { };

#define REDASM_ENDIANNESS_OF(endianness, suffix) template<> struct endianness_of<s16##suffix> { static constexpr endianness_t value = endianness; }; \
                                                 template<> struct endianness_of<s32##suffix> { static constexpr endianness_t value = endianness; }; \
                                                 template<> struct endianness_of<s64##suffix> { static constexpr endianness_t value = endianness; }; \
                                                 template<> struct endianness_of<u16##suffix> { static constexpr endianness_t value = endianness; }; \
                                                 template<> struct endianness_of<u32##suffix> { static constexpr endianness_t value = endianness; }; \
                                                 template<> struct endianness_of<u64##suffix> { static constexpr endianness_t value = endianness; };

namespace REDasm {

typedef u32 endianness_t;

namespace Endianness {

namespace Impl { // https://stackoverflow.com/questions/1583791/constexpr-and-endianness

static constexpr u32 TestValue         = 0x01020304;
static constexpr u32 Test              = static_cast<const u8&>(TestValue);
static constexpr bool TestLittleEndian = Test == 0x04;
static constexpr bool TestBigEndian    = Test == 0x02;

static_assert(TestLittleEndian != TestBigEndian, "Unsupported platform endianness");

} // namespace Impl

enum { LittleEndian = 0, BigEndian = 1 };

constexpr bool isLittleEndian = Impl::TestLittleEndian;
constexpr bool isBigEndian = Impl::TestBigEndian;
constexpr int current = Endianness::isLittleEndian ? Endianness::LittleEndian : Endianness::BigEndian;

constexpr bool needsSwap(endianness_t endianness) { return Endianness::current != endianness; }

template<typename T> struct is_little_endian: public std::false_type { };
template<typename T> struct is_big_endian: public std::false_type { };
template<typename T> struct endianness_of { static constexpr endianness_t value = Endianness::current; };

REDASM_ENDIAN_TYPES(is_little_endian, std::true_type,  le)
REDASM_ENDIAN_TYPES(is_little_endian, std::false_type, be)
REDASM_ENDIAN_TYPES(is_big_endian,    std::false_type, le)
REDASM_ENDIAN_TYPES(is_big_endian,    std::true_type,  be)

REDASM_ENDIAN_TYPES_COND(is_little_endian, Endianness::isLittleEndian);
REDASM_ENDIAN_TYPES_COND(is_big_endian, Endianness::isBigEndian);

REDASM_ENDIANNESS_OF(Endianness::LittleEndian, le);
REDASM_ENDIANNESS_OF(Endianness::BigEndian, be);

template<typename T> endianness_t endianness()
{
    if(is_little_endian<T>())
        return Endianness::LittleEndian;

    return Endianness::BigEndian;
}

template<typename T> T swap(T v) {
   u8* p = reinterpret_cast<u8*>(&v);
   std::reverse(p, p + sizeof(T));
   return v;
}

template<typename T> T checkSwap(T v, endianness_t endianness) {
   if(!Endianness::needsSwap(endianness))
       return v;

   return Endianness::swap(v);
}

template<typename T> size_t swap(u8* data, size_t size) {
    T* pendingdata = reinterpret_cast<T*>(data);
    s64 pendingsize = static_cast<s64>(size);

    while(pendingsize >= sizeof(T))
    {
        *pendingdata = Endianness::swap<T>(*pendingdata);
        pendingdata++;
        pendingsize -= sizeof(T);
    }

    return pendingsize;
}

template<typename T> T cfbe(T v) { return checkSwap(v, Endianness::BigEndian); }                // Convert FROM BigEndian TO PlatformEndian
template<typename T> T cfle(T v) { return checkSwap(v, Endianness::LittleEndian); }             // Convert FROM LittleEndian TO PlatformEndian
template<typename T> T cfbe(const T* data) { return cfbe(*data); }
template<typename T> T cfle(const T* data) { return cfle(*data); }

} // namespace Endianness

template<typename T, typename R = typename platform_integral_type<T>::type> R endianness_cast(const T& t)
{
    if(Endianness::endianness<T>() == Endianness::current)
        return static_cast<R>(t);

    return Endianness::swap(static_cast<R>(t));
}

#define E_(x) endianness_cast(x)

} // namespace REDasm

#endif // ENDIANNESS_H
