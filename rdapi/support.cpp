#include "support.h"
#include <rdcore/support/demangler.h>
#include <rdcore/support/utils.h>
#include <rdcore/disassembler.h>
#include <rdcore/context.h>

const char* RD_ToHexBits(size_t value, size_t bits, bool withprefix)
{
    // Fixed internal storage
    static std::string s;

    if(!bits) bits = rd_ctx->disassembler()->assembler()->bits;

    s = Utils::hex(value, bits, withprefix);
    return s.c_str();
}

const char* RD_ToHex(size_t value) { return RD_ToHexBits(value, 0, false); }

const char* RD_ToStringBase(size_t value, size_t base, size_t width, char fill)
{
    // Fixed internal storage
    static std::string s;
    s = Utils::number(value, base, width, fill);
    return s.c_str();
}

const char* RD_ToString(size_t value) { return RD_ToStringBase(value, 10, 0, '0'); }
u8* RD_RelPointer(void* ptr, size_t offset) { return Utils::relpointer(ptr, offset); }
bool RD_InRange(address_t address, address_t start, address_t end) { return ((address >= start) && (address < end));  }
bool RD_InRangeSize(address_t address, address_t start, address_t size) { return ((address >= start) && (address < (start + size))); }
const char* RD_Demangle(const char* name) { return name ? Demangler::demangled(name) : nullptr; }
