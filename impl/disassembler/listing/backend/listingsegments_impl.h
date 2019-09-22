#pragma once

#include "../../../types/containers/templates/sortedlist_template.h"
#include <redasm/disassembler/listing/backend/listingsegments.h>

namespace REDasm {

struct SegmentComparator
{
    bool operator()(const Segment& segment1, const Segment& segment2) const {
        return segment1.address < segment2.address;
    }
};

class ListingSegmentsImpl: public SortedListTemplate<Segment, SegmentComparator>
{
    public:
        ListingSegmentsImpl();
};

} // namespace REDasm
