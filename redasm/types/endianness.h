#ifndef REDASM_ENDIANNESS_H
#define REDASM_ENDIANNESS_H

#include <algorithm>
#include <type_traits>
#include "base_types.h"
#include "numeric_type.h"

#define REDASM_ENDIAN_TYPES(suffix, endianness) typedef endianness_numeric_type<s16, endianness> s16##suffix; \
                                                typedef endianness_numeric_type<s32, endianness> s32##suffix; \
                                                typedef endianness_numeric_type<s64, endianness> s64##suffix; \
                                                typedef endianness_numeric_type<u16, endianness> u16##suffix; \
                                                typedef endianness_numeric_type<u32, endianness> u32##suffix; \
                                                typedef endianness_numeric_type<u64, endianness> u64##suffix;

#define REDASM_ENDIAN_CHECK(name, cond, suffix) template<> struct name<s16##suffix>: public std::conditional<cond, std::true_type, std::false_type>::type { }; \
                                                template<> struct name<s32##suffix>: public std::conditional<cond, std::true_type, std::false_type>::type { }; \
                                                template<> struct name<s64##suffix>: public std::conditional<cond, std::true_type, std::false_type>::type { }; \
                                                template<> struct name<u16##suffix>: public std::conditional<cond, std::true_type, std::false_type>::type { }; \
                                                template<> struct name<u32##suffix>: public std::conditional<cond, std::true_type, std::false_type>::type { }; \
                                                template<> struct name<u64##suffix>: public std::conditional<cond, std::true_type, std::false_type>::type { };

#define REDASM_PLATFORM_NUMERIC(suffix) template<> struct platform_numeric_type<u16##suffix> { typedef u16 type; }; \
                                        template<> struct platform_numeric_type<u32##suffix> { typedef u32 type; }; \
                                        template<> struct platform_numeric_type<u64##suffix> { typedef u64 type; }; \
                                        template<> struct platform_numeric_type<s16##suffix> { typedef s16 type; }; \
                                        template<> struct platform_numeric_type<s32##suffix> { typedef s32 type; }; \

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

enum: u32 { LittleEndian = 0, BigEndian = 1 };

constexpr bool isLittleEndian = Impl::TestLittleEndian;
constexpr bool isBigEndian = Impl::TestBigEndian;
constexpr int current = Endianness::isLittleEndian ? Endianness::LittleEndian : Endianness::BigEndian;
constexpr bool needsSwap(endianness_t endianness) { return Endianness::current != endianness; }

template<typename T> T swap(T v) { u8* p = reinterpret_cast<u8*>(&v); std::reverse(p, p + sizeof(T)); return v; }
template<typename T> constexpr T checkSwap(T v, endianness_t endianness) { return Endianness::needsSwap(endianness) ? Endianness::swap(v) : v; }

template<typename T, endianness_t endianness_of> class endianness_numeric_type: public numeric_type<T>
{
    public:
        endianness_numeric_type(const T& val): numeric_type<T>(val) { this->m_val = Endianness::checkSwap(this->m_val, endianness_of); }
};

template<typename T> struct is_little_endian: public std::false_type { };
template<typename T> struct is_big_endian: public std::false_type { };

REDASM_ENDIAN_TYPES(le, Endianness::LittleEndian)
REDASM_ENDIAN_TYPES(be, Endianness::BigEndian)

REDASM_ENDIAN_CHECK(is_little_endian, true, le)
REDASM_ENDIAN_CHECK(is_little_endian, false, be)
REDASM_ENDIAN_CHECK(is_big_endian, false, le)
REDASM_ENDIAN_CHECK(is_big_endian, true, be)

REDASM_ENDIAN_CHECK(is_little_endian, Endianness::isLittleEndian,)
REDASM_ENDIAN_CHECK(is_big_endian, Endianness::isBigEndian,)

template<typename T> constexpr endianness_t endianness_of() { return is_little_endian<T>() ?  Endianness::LittleEndian : Endianness::BigEndian; }

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

using u16le = Endianness::u16le;
using u32le = Endianness::u32le;
using u64le = Endianness::u64le;
using s16le = Endianness::s16le;
using s32le = Endianness::s32le;
using s64le = Endianness::s64le;
using u16be = Endianness::u16be;
using u32be = Endianness::u32be;
using u64be = Endianness::u64be;
using s16be = Endianness::s16be;
using s32be = Endianness::s32be;
using s64be = Endianness::s64be;

template<typename T> struct platform_numeric_type { };

REDASM_PLATFORM_NUMERIC(le) // Little Endian Platform Types
REDASM_PLATFORM_NUMERIC(be) // Big Endian Platform Types

template<typename T, typename R = typename platform_numeric_type<T>::type > constexpr R endianness_cast(const T& t) { return (Endianness::endianness_of<T>() == Endianness::current) ? static_cast<R>(t) : Endianness::swap(static_cast<R>(t)); }

} // namespace REDasm

#endif // ENDIANNESS_H
