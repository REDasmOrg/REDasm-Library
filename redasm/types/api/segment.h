#pragma once

#include "api_base.h"
#include "../macros.h"
#include "../string.h"

namespace REDasm {

enum class SegmentType: type_t
{
    None        = 0x00000000,
    Code        = 0x00000001,
    Data        = 0x00000002,
    Bss         = 0x00000004,

    DataAndCode = Data | Code,
};

ENUM_FLAGS_OPERATORS(SegmentType)

struct Segment
{
    char name_[DEFAULT_NAME_SIZE];
    offset_t offset, endoffset;
    address_t address, endaddress;
    SegmentType type;
    size_t coveragebytes{REDasm::npos};

    Segment() = default;
    Segment(const String& name, offset_t offset, address_t addr, u64 psize, u64 vsize, SegmentType t);
    String name() const;
    u64 size() const;
    u64 rawSize() const;
    bool empty() const;
    bool contains(address_t address) const;
    bool containsOffset(offset_t off) const;
    bool is(SegmentType t) const;
    bool isPureCode() const;
};

FORCE_STANDARD_LAYOUT(Segment);

} // namespace REDasm
