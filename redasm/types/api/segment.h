#pragma once

#include "api_base.h"
#include "../../macros.h"
#include "../string.h"

namespace REDasm {

struct Segment
{
    enum Type : type_t {
        T_None        = 0x00000000,
        T_Code        = 0x00000001,
        T_Data        = 0x00000002,
        T_Bss         = 0x00000004,

        T_DataAndCode = T_Data | T_Code,
    };

    char name_[DEFAULT_NAME_SIZE];
    offset_t offset, endoffset;
    address_t address, endaddress;
    type_t type;
    size_t coveragebytes{REDasm::npos};

    Segment() = default;
    Segment(const String& name, offset_t offset, address_t addr, u64 psize, u64 vsize, type_t t);
    String name() const;
    u64 size() const;
    u64 rawSize() const;
    bool empty() const;
    bool contains(address_t address) const;
    bool containsOffset(offset_t off) const;
    bool is(type_t t) const;
    bool isPureCode() const;
};

FORCE_STANDARD_LAYOUT(Segment);

} // namespace REDasm
