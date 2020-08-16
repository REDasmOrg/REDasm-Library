#include "types.h"
#include <rdcore/document/backend/segmentcontainer.h>

size_t RDSegment_RawSize(const RDSegment* s) { return SegmentContainer::offsetSize(*s); }
size_t RDSegment_Size(const RDSegment* s) { return SegmentContainer::addressSize(*s); }
bool RDSegment_ContainsAddress(const RDSegment* s, rd_address address) { return SegmentContainer::containsAddress(s, address); }
bool RDSegment_ContainsOffset(const RDSegment* s, rd_offset offset) { return SegmentContainer::containsOffset(s, offset); }
