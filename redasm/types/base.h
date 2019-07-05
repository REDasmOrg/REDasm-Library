#pragma once

#include <cstdint>
#include <cstddef>

#define REDASM_TYPE_BITS(bits, S, U) template<> struct type_from_bits_impl<bits> { \
                                                    typedef S signed_type; \
                                                    typedef U unsigned_type; \
                                                };

#define REDASM_SIGNED_OF(U, S)   template<> struct signed_of<U> { typedef S type; };
#define REDASM_UNSIGNED_OF(S, U) template<> struct unsigned_of<S> { typedef U type; };

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

typedef u64 address_t;
typedef u64 offset_t;
typedef u64 instruction_id_t;
typedef s64 register_id_t;
typedef u64 tag_t;
typedef u32 object_id_t;

namespace REDasm {

constexpr size_t npos = static_cast<size_t>(-1);

template<int bits> struct type_from_bits_impl { };

template<typename T> struct location_of {
    bool valid;
    T value;

    location_of(): valid(false), value(0) { }
    location_of(T val, bool v): valid(v), value(val) { }
    location_of(const location_of<T>&) = default;
    constexpr operator T() const { return value; }
};

template<typename T> struct unsigned_of { };
template<typename T> struct signed_of { };

template<typename T> location_of<T> make_location(T val, bool v = true) { return location_of<T>(val, v); }
template<typename T> location_of<T> invalid_location() { return location_of<T>(0, false); }

REDASM_TYPE_BITS(8,  s8,  u8)
REDASM_TYPE_BITS(16, s16, u16)
REDASM_TYPE_BITS(32, s32, u32)
REDASM_TYPE_BITS(64, s64, u64)

REDASM_SIGNED_OF(u8,  s8)
REDASM_SIGNED_OF(u16, s16)
REDASM_SIGNED_OF(u32, s32)
REDASM_SIGNED_OF(u64, s64)
REDASM_SIGNED_OF(s8,  s8)
REDASM_SIGNED_OF(s16, s16)
REDASM_SIGNED_OF(s32, s32)
REDASM_SIGNED_OF(s64, s64)

REDASM_UNSIGNED_OF(s8,  u8)
REDASM_UNSIGNED_OF(s16, u16)
REDASM_UNSIGNED_OF(s32, u32)
REDASM_UNSIGNED_OF(s64, u64)
REDASM_UNSIGNED_OF(u8,  u8)
REDASM_UNSIGNED_OF(u16, u16)
REDASM_UNSIGNED_OF(u32, u32)
REDASM_UNSIGNED_OF(u64, u64)

#define type_from_bits(bits, t) typename type_from_bits_impl<bits>::t

} // namespace REDasm

typedef REDasm::location_of<offset_t> offset_location;
typedef REDasm::location_of<address_t> address_location;
