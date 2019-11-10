#include "segment.h"

namespace REDasm {

Segment::Segment(const String &name, offset_t off, address_t addr, u64 psize, u64 vsize, SegmentType t)
{
    name.copy(this->name_, DEFAULT_NAME_SIZE);
    this->type = t;
    this->offset = off;
    this->endoffset = off + psize;
    this->address = addr;
    this->endaddress = addr + vsize;
}

String Segment::name() const { return name_; }
u64 Segment::size() const { return (address > endaddress) ? 0 : (endaddress - address); }
u64 Segment::rawSize() const { return (offset > endoffset) ? 0 : (endoffset - offset); }
bool Segment::empty() const { return !this->size(); }
bool Segment::contains(address_t addr) const { return (addr >= this->address) && (addr < this->endaddress); }
bool Segment::containsOffset(offset_t off) const { return !this->is(SegmentType::Bss) && ((off >= this->offset) && (off < this->endoffset)); }
bool Segment::is(SegmentType t) const { return type & t; }
bool Segment::isPureCode() const { return type == SegmentType::Code; }

} // namespace REDasm
