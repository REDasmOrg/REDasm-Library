#include "utils.h"
#include <rdcore/support/demangler.h>
#include <rdcore/support/utils.h>
#include <rdcore/support/endian.h>
#include <rdcore/disassembler.h>
#include <rdcore/context.h>

const char* RD_ToHexBits(size_t value, size_t bits, bool withprefix)
{
    // Fixed internal storage
    static std::string s;

    if(bits == RD_NPOS) bits = rd_ctx->disassembler()->assembler()->bits;

    s = Utils::hex(value, bits, withprefix);
    return s.c_str();
}

const char* RD_ToHexAuto(size_t value) { return RD_ToHexBits(value, RD_NPOS, false); }
const char* RD_ToHex(size_t value) { return RD_ToHexBits(value, 0, false); }

const char* RD_ToStringBase(size_t value, size_t base, size_t width, char fill)
{
    // Fixed internal storage
    static std::string s;
    s = Utils::number(value, base, width, fill);
    return s.c_str();
}

const char* RD_Trampoline(const char* name)
{
    // Fixed internal storage
    static std::string s;
    s = Utils::trampoline(name);
    return s.c_str();
}

const char* RD_ToString(size_t value) { return RD_ToStringBase(value, 10, 0, '0'); }
u8* RD_RelPointer(void* ptr, size_t offset) { return Utils::relpointer(ptr, offset); }
bool RD_InRange(address_t address, address_t start, address_t end) { return ((address >= start) && (address < end));  }
bool RD_InRangeSize(address_t address, address_t start, address_t size) { return ((address >= start) && (address < (start + size))); }
const char* RD_Demangle(const char* name) { return name ? Demangler::demangled(name) : nullptr; }
u16 RD_Swap16(u16 val) { return Endian::swap16(val); }
u32 RD_Swap32(u16 val) { return Endian::swap32(val); }
u64 RD_Swap64(u16 val) { return Endian::swap64(val); }
u16 RD_ToLittleEndian16(u16 val) { return Endian::tolittlendian16(val); }
u32 RD_ToLittleEndian32(u32 val) { return Endian::tolittlendian32(val); }
u64 RD_ToLittleEndian64(u64 val) { return Endian::tolittlendian64(val); }
u16 RD_ToBigEndian16(u16 val) { return Endian::tobigendian16(val); }
u32 RD_ToBigEndian32(u32 val) { return Endian::tobigendian32(val); }
u64 RD_ToBigEndian64(u64 val) { return Endian::tobigendian64(val); }
