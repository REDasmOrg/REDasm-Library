#pragma once

#include "macros.h"
#include "types.h"

enum RDEndianness {
    Endianness_Invalid,
    Endianness_Little,
    Endianness_Big,
};

struct RDInstruction;

RD_API_EXTERN_C const char* RD_ToHexBits(size_t value, size_t bits, bool withprefix);
RD_API_EXTERN_C const char* RD_ToHexAuto(size_t value);
RD_API_EXTERN_C const char* RD_ToHex(size_t value);
RD_API_EXTERN_C const char* RD_ToStringBase(size_t value, size_t base, size_t width, char fill);
RD_API_EXTERN_C const char* RD_ToString(size_t value);
RD_API_EXTERN_C const char* RD_Trampoline(const char* name);
RD_API_EXTERN_C const char* RD_Demangle(const char* name);
RD_API_EXTERN_C u8* RD_RelPointer(void* ptr, size_t offset);
RD_API_EXTERN_C bool RD_InRange(address_t address, address_t start, address_t end);
RD_API_EXTERN_C bool RD_InRangeSize(address_t address, address_t start, address_t size);
RD_API_EXTERN_C u16 RD_Swap16(u16 val);
RD_API_EXTERN_C u32 RD_Swap32(u32 val);
RD_API_EXTERN_C u64 RD_Swap64(u64 val);
RD_API_EXTERN_C u16 RD_ToLittleEndian16(u16 val);
RD_API_EXTERN_C u32 RD_ToLittleEndian32(u32 val);
RD_API_EXTERN_C u64 RD_ToLittleEndian64(u64 val);
RD_API_EXTERN_C u16 RD_ToBigEndian16(u16 val);
RD_API_EXTERN_C u32 RD_ToBigEndian32(u32 val);
RD_API_EXTERN_C u64 RD_ToBigEndian64(u64 val);

#ifdef __cplusplus
  #include <string>
  #define rd_str(s)                                 std::string(s).c_str()
  #define rd_tohexbits(value, bits, withprefix)     std::string(RD_ToHexBits(value, bits, withprefix))
  #define rd_tohex(value)                           std::string(RD_ToHex(value))
  #define rd_tohexauto(value)                       std::string(RD_ToHexAuto(value))
  #define rd_tostringbase(value, base, width, fill) std::string(RD_ToStringBase(value, base, width, fill))
  #define rd_tostring(value)                        std::string(RD_ToString(value))
#endif
