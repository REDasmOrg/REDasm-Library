#pragma once

#include "../../../types/api.h"
#include "../../../macros.h"
#include "../../../pimpl.h"

namespace REDasm {

class ListingSegmentsImpl;

class LIBREDASM_API ListingSegments
{
    PIMPL_DECLARE_P(ListingSegments)
    PIMPL_DECLARE_PRIVATE(ListingSegments)

    public:
        ListingSegments();
        bool empty() const;
        size_t size() const;
        const Segment* at(size_t idx) const;
        const Segment* find(const String& name) const;
        const Segment* find(address_t address) const;
        bool insert(const String& name, offset_t offset, address_t address, u64 size, SegmentType type);
        bool insert(const String& name, offset_t offset, address_t address, u64 psize, u64 vsize, SegmentType type);
};

} // namespace REDasm
