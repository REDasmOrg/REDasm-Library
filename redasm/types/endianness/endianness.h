#ifndef REDASM_ENDIANNESS_H
#define REDASM_ENDIANNESS_H

#include <algorithm>
#include <type_traits>
#include "../base_types.h"
#include "../numeric_type.h"
#include "endianness_base.h"

#define REDASM_ENDIANNESS_OF(T, b, e) template<> struct of<T> { \
                                           typedef T type; \
                                           typedef b base; \
                                           static constexpr endianness_t value = e; \
                                           static constexpr bool isLittleEndian = e == Endianness::LittleEndian; \
                                           static constexpr bool isBigEndian = e == Endianness::BigEndian; \
                                           static constexpr bool needsSwap = e != Endianness::current; \
                                      };

namespace REDasm {

namespace Endianness {

typedef numeric_type<s16, Endianness::LittleEndian> s16le;
typedef numeric_type<s32, Endianness::LittleEndian> s32le;
typedef numeric_type<s64, Endianness::LittleEndian> s64le;
typedef numeric_type<u16, Endianness::LittleEndian> u16le;
typedef numeric_type<u32, Endianness::LittleEndian> u32le;
typedef numeric_type<u64, Endianness::LittleEndian> u64le;
typedef numeric_type<s16, Endianness::BigEndian> s16be;
typedef numeric_type<s32, Endianness::BigEndian> s32be;
typedef numeric_type<s64, Endianness::BigEndian> s64be;
typedef numeric_type<u16, Endianness::BigEndian> u16be;
typedef numeric_type<u32, Endianness::BigEndian> u32be;
typedef numeric_type<u64, Endianness::BigEndian> u64be;

template<typename T> struct of { };

REDASM_ENDIANNESS_OF(s8,  s8,  Endianness::current)
REDASM_ENDIANNESS_OF(s16, s16, Endianness::current)
REDASM_ENDIANNESS_OF(s32, s32, Endianness::current)
REDASM_ENDIANNESS_OF(s64, s64, Endianness::current)
REDASM_ENDIANNESS_OF(u8,  u8,  Endianness::current)
REDASM_ENDIANNESS_OF(u16, u16, Endianness::current)
REDASM_ENDIANNESS_OF(u32, u32, Endianness::current)
REDASM_ENDIANNESS_OF(u64, u64, Endianness::current)
REDASM_ENDIANNESS_OF(s16le, s16, Endianness::LittleEndian)
REDASM_ENDIANNESS_OF(s32le, s32, Endianness::LittleEndian)
REDASM_ENDIANNESS_OF(s64le, s64, Endianness::LittleEndian)
REDASM_ENDIANNESS_OF(u16le, u16, Endianness::LittleEndian)
REDASM_ENDIANNESS_OF(u32le, u32, Endianness::LittleEndian)
REDASM_ENDIANNESS_OF(u64le, u64, Endianness::LittleEndian)
REDASM_ENDIANNESS_OF(s16be, s16, Endianness::BigEndian)
REDASM_ENDIANNESS_OF(s32be, s32, Endianness::BigEndian)
REDASM_ENDIANNESS_OF(s64be, s64, Endianness::BigEndian)
REDASM_ENDIANNESS_OF(u16be, u16, Endianness::BigEndian)
REDASM_ENDIANNESS_OF(u32be, u32, Endianness::BigEndian)
REDASM_ENDIANNESS_OF(u64be, u64, Endianness::BigEndian)

template<typename T> constexpr T checkSwap(T v, endianness_t endianness) { return Endianness::of<T>::needsSwap ? Endianness::swap(v) : v; }

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

template<typename T, typename R = typename Endianness::of<T>::base > constexpr R endianness_cast(const T& t) {
    return Endianness::of<T>::needsSwap ? static_cast<R>(t) : Endianness::swap(static_cast<R>(t));
}

} // namespace REDasm

#endif // ENDIANNESS_H
