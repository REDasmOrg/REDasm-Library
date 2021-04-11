#include "types.h"
#include <rdcore/document/model/addressspace.h>
#include <rdcore/support/utils.h>

size_t RDSegment_RawSize(const RDSegment* s) { return AddressSpace::offsetSize(*s); }
size_t RDSegment_Size(const RDSegment* s) { return AddressSpace::addressSize(*s); }
bool RDSegment_ContainsAddress(const RDSegment* s, rd_address address) { return AddressSpace::containsAddress(s, address); }
bool RDSegment_ContainsOffset(const RDSegment* s, rd_offset offset) { return AddressSpace::containsOffset(s, offset); }
u64 ULEB128_Decode(const u8* uleb128, size_t* c) { return uleb128 ? Utils::uleb128Decode(uleb128, c) : 0; }
s64 LEB128_Decode(const u8* leb128, size_t size, size_t* c) { return leb128 ? Utils::leb128Decode(leb128, size, c) : 0; }
