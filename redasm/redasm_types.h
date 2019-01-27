#ifndef REDASM_TYPES_H
#define REDASM_TYPES_H

#include <cstdint>
#include <cstddef>

#define strong_typedef(T, alias) enum alias: T;

#define REDASM_TYPES(suffix) strong_typedef(u16, u16##suffix); \
                             strong_typedef(u32, u32##suffix); \
                             strong_typedef(u64, u64##suffix); \
                             strong_typedef(s16, s16##suffix); \
                             strong_typedef(s32, s32##suffix); \
                             strong_typedef(s64, s64##suffix);

template<typename T> struct platform_integral_type { };

#define REDASM_BASE_TYPE(suffix) template<> struct platform_integral_type<u16##suffix> { typedef u16 type; }; \
                                 template<> struct platform_integral_type<u32##suffix> { typedef u32 type; }; \
                                 template<> struct platform_integral_type<u64##suffix> { typedef u64 type; }; \
                                 template<> struct platform_integral_type<s16##suffix> { typedef s16 type; }; \
                                 template<> struct platform_integral_type<s32##suffix> { typedef s32 type; }; \
                                 template<> struct platform_integral_type<s64##suffix> { typedef u64 type; };

typedef uint8_t  u8;
typedef int8_t   s8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

REDASM_TYPES(le)     // Little Endian Specialized Types
REDASM_TYPES(be)     // Big Endian Specialized Types
REDASM_BASE_TYPE(le) // Little Endian Platform Types
REDASM_BASE_TYPE(be) // Big Endian Platform Types

typedef s64 register_t;
typedef u64 address_t;
typedef u64 offset_t;
typedef u64 instruction_id_t;

#endif // REDASM_TYPES_H
