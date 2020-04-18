#pragma once

#include "macros.h"
#include "types.h"

struct RDInstruction;

RD_API_EXTERN_C const char* RD_ToHexBits(size_t value, size_t bits, bool withprefix);
RD_API_EXTERN_C const char* RD_ToHex(size_t value);
RD_API_EXTERN_C const char* RD_ToStringBase(size_t value, size_t base, size_t width, char fill);
RD_API_EXTERN_C const char* RD_ToString(size_t value);
RD_API_EXTERN_C const char* RD_Demangle(const char* name);
RD_API_EXTERN_C u8* RD_RelPointer(void* ptr, size_t offset);
RD_API_EXTERN_C bool RD_InRange(address_t address, address_t start, address_t end);
RD_API_EXTERN_C bool RD_InRangeSize(address_t address, address_t start, address_t size);

#ifdef __cplusplus
  #include <string>
  #define rd_str(s)                                 std::string(s).c_str()
  #define rd_tohexbits(value, bits, withprefix)     std::string(RD_ToHexBits(value, bits, withprefix))
  #define rd_tohex(value)                           std::string(RD_ToHex(value))
  #define rd_tostringbase(value, base, width, fill) std::string(RD_ToStringBase(value, base, width, fill))
  #define rd_tostring(value)                        std::string(RD_ToString(value))
#endif
