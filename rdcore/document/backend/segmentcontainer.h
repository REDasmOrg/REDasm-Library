#pragma once

#include <rdapi/document/document.h>
#include "../../containers.h"
#include <set>

struct SegmentSorter
{
    bool operator()(const RDSegment& segment1, const RDSegment& segment2) const {
        return segment1.address < segment2.address;
    }
};

struct SegmentComparator
{
    bool operator()(const RDSegment& segment1, const RDSegment& segment2) const {
        return std::tie(segment1.name, segment1.flags, segment1.offset, segment1.address, segment1.endoffset, segment1.endaddress) ==
               std::tie(segment2.name, segment2.flags, segment2.offset, segment2.address, segment2.endoffset, segment2.endaddress);
    }
};

class SegmentContainer: public SortedContainer<RDSegment, SegmentSorter, SegmentComparator>
{
    public:
        SegmentContainer() = default;
        void removeAddress(address_t address);
        bool find(address_t address, RDSegment* segment) const;
        bool findOffset(offset_t address, RDSegment* segment) const;

    public:
        static size_t addressSize(const RDSegment& segment);
        static size_t offsetSize(const RDSegment& segment);
        static bool containsAddress(const RDSegment* segment, address_t address);
        static bool containsOffset(const RDSegment* segment, offset_t offset);
};
