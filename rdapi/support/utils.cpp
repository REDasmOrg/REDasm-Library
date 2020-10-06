#include "utils.h"
#include <rdcore/support/demangler.h>
#include <rdcore/support/utils.h>
#include <rdcore/support/endian.h>
#include <rdcore/disassembler.h>
#include <rdcore/context.h>
#include <cstring>

const char* RD_ToHexBits(size_t value, size_t bits, bool withprefix)
{
    // Fixed internal storage
    static std::string s;

    if(bits == RD_NPOS) bits = 0; //FIXME: rd_ctx->disassembler()->assembler()->bits();

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

const char* RD_Thunk(const char* name)
{
    // Fixed internal storage
    static std::string s;
    s = Utils::thunk(name);
    return s.c_str();
}

const char* RD_ToString(size_t value) { return RD_ToStringBase(value, 10, 0, '0'); }
u8* RD_RelPointer(void* ptr, size_t offset) { return Utils::relpointer(ptr, offset); }
bool RD_InRange(rd_address address, rd_address start, rd_address end) { return ((address >= start) && (address < end));  }
bool RD_InRangeSize(rd_address address, rd_address start, rd_address size) { return ((address >= start) && (address < (start + size))); }
const char* RD_Demangle(const char* name) { return name ? Demangler::demangled(name) : nullptr; }
intptr_t RD_SignExt(uintptr_t val, int valbits) { return Utils::signext(val, valbits); }
u16 RD_Swap16(u16 hostval) { return Endian::swap16(hostval); }
u32 RD_Swap32(u32 hostval) { return Endian::swap32(hostval); }
u64 RD_Swap64(u64 hostval) { return Endian::swap64(hostval); }
u16 RD_Rol16(u16 val, u16 amt) { return Utils::rol(val, amt); }
u32 RD_Rol32(u32 val, u32 amt) { return Utils::rol(val, amt); }
u64 RD_Rol64(u64 val, u64 amt) { return Utils::rol(val, amt); }
u16 RD_Ror16(u16 val, u16 amt) { return Utils::ror(val, amt); }
u32 RD_Ror32(u32 val, u32 amt) { return Utils::ror(val, amt); }
u64 RD_Ror64(u64 val, u64 amt) { return Utils::ror(val, amt); }
u16 RD_FromLittleEndian16(u16 hostval) { return Endian::fromlittleendian16(hostval); }
u32 RD_FromLittleEndian32(u32 hostval) { return Endian::fromlittleendian32(hostval); }
u64 RD_FromLittleEndian64(u64 hostval) { return Endian::fromlittleendian64(hostval); }
u16 RD_FromBigEndian16(u16 hostval) { return Endian::frombigendian16(hostval); }
u32 RD_FromBigEndian32(u32 hostval) { return Endian::frombigendian32(hostval); }
u64 RD_FromBigEndian64(u64 hostval) { return Endian::frombigendian64(hostval); }
