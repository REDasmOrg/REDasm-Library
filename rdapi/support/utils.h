#pragma once

#include "../macros.h"
#include "../types.h"

enum RDEndianness {
    Endianness_Invalid,
    Endianness_Little,
    Endianness_Big,
};

typedef u16 (*Swap16_Callback)(u16);
typedef u32 (*Swap32_Callback)(u32);
typedef u64 (*Swap64_Callback)(u64);

struct RDContext;

RD_API_EXPORT const char* RD_ToHexBits(size_t value, size_t bits, bool withprefix);
RD_API_EXPORT const char* RD_ToHexAuto(const RDContext* ctx, size_t value);
RD_API_EXPORT const char* RD_ToHex(size_t value);
RD_API_EXPORT const char* RD_ToStringBase(size_t value, size_t base, size_t width, char fill);
RD_API_EXPORT const char* RD_ToString(size_t value);
RD_API_EXPORT const char* RD_Thunk(const char* name);
RD_API_EXPORT const char* RD_Demangle(const char* name);
RD_API_EXPORT u8* RD_RelPointer(void* ptr, size_t offset);
RD_API_EXPORT bool RD_InRange(rd_address address, rd_address start, rd_address end);
RD_API_EXPORT bool RD_InRangeSize(rd_address address, rd_address start, rd_address size);
RD_API_EXPORT intptr_t RD_SignExt(uintptr_t val, int valbits);
RD_API_EXPORT size_t* RD_HashCombine(size_t* h, size_t v);
RD_API_EXPORT u32 RD_Adler32(const u8* data, size_t size);
RD_API_EXPORT u32 RD_Crc32(const u8* data, size_t size);
RD_API_EXPORT u16 RD_Swap16(u16 hostval);
RD_API_EXPORT u32 RD_Swap32(u32 hostval);
RD_API_EXPORT u64 RD_Swap64(u64 hostval);
RD_API_EXPORT u16 RD_Rol16(u16 val, u16 amt);
RD_API_EXPORT u32 RD_Rol32(u32 val, u32 amt);
RD_API_EXPORT u64 RD_Rol64(u64 val, u64 amt);
RD_API_EXPORT u16 RD_Ror16(u16 val, u16 amt);
RD_API_EXPORT u32 RD_Ror32(u32 val, u32 amt);
RD_API_EXPORT u64 RD_Ror64(u64 val, u64 amt);
RD_API_EXPORT u16 RD_FromLittleEndian16(u16 hostval);
RD_API_EXPORT u32 RD_FromLittleEndian32(u32 hostval);
RD_API_EXPORT u64 RD_FromLittleEndian64(u64 hostval);
RD_API_EXPORT u16 RD_FromBigEndian16(u16 hostval);
RD_API_EXPORT u32 RD_FromBigEndian32(u32 hostval);
RD_API_EXPORT u64 RD_FromBigEndian64(u64 hostval);
RD_API_EXPORT bool RD_StrEquals(const char* s1, const char* s2);

// Some function aliases for endianness conversion
inline u16 RD_ToLittleEndian16(u16 hostval) { return RD_FromLittleEndian16(hostval); }
inline u32 RD_ToLittleEndian32(u32 hostval) { return RD_FromLittleEndian32(hostval); }
inline u64 RD_ToLittleEndian64(u64 hostval) { return RD_FromLittleEndian64(hostval); }
inline u16 RD_ToBigEndian16(u16 hostval) { return RD_FromBigEndian16(hostval); }
inline u32 RD_ToBigEndian32(u32 hostval) { return RD_FromBigEndian32(hostval); }
inline u64 RD_ToBigEndian64(u64 hostval) { return RD_FromBigEndian64(hostval); }

// Shorter from-endianness conversion
#define rd_fromle16 RD_FromLittleEndian16
#define rd_fromle32 RD_FromLittleEndian32
#define rd_fromle64 RD_FromLittleEndian64
#define rd_frombe16 RD_FromBigEndian16
#define rd_frombe32 RD_FromBigEndian32
#define rd_frombe64 RD_FromBigEndian64

// Shorter to-endianness conversion
#define rd_tole16 RD_ToLittleEndian16
#define rd_tole32 RD_ToLittleEndian32
#define rd_tole64 RD_ToLittleEndian64
#define rd_tobe16 RD_ToBigEndian16
#define rd_tobe32 RD_ToBigEndian32
#define rd_tobe64 RD_ToBigEndian64

#ifdef __cplusplus
  #include <string>
  #define rd_str(s)                                 std::string(s).c_str()
  #define rd_tohexbits(value, bits, withprefix)     std::string(RD_ToHexBits(value, bits, withprefix))
  #define rd_tohex(value)                           std::string(RD_ToHex(value))
  #define rd_tohexauto(ctx, value)                  std::string(RD_ToHexAuto(ctx, value))
  #define rd_tostringbase(value, base, width, fill) std::string(RD_ToStringBase(value, base, width, fill))
  #define rd_tostring(value)                        std::string(RD_ToString(value))
#endif
