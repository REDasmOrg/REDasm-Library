#include "types.h"
#include <rdcore/document/model/addressspace.h>

size_t RDSegment_RawSize(const RDSegment* s) { return AddressSpace::offsetSize(*s); }
size_t RDSegment_Size(const RDSegment* s) { return AddressSpace::addressSize(*s); }
bool RDSegment_ContainsAddress(const RDSegment* s, rd_address address) { return AddressSpace::containsAddress(s, address); }
bool RDSegment_ContainsOffset(const RDSegment* s, rd_offset offset) { return AddressSpace::containsOffset(s, offset); }
