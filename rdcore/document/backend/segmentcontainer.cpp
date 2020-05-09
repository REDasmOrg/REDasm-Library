#include "segmentcontainer.h"
#include <algorithm>

void SegmentContainer::removeAddress(address_t address)
{
    auto it = std::find_if(m_container.begin(), m_container.end(), [address](const RDSegment& segment) {
        return SegmentContainer::containsAddress(&segment, address);
    });

    if(it != m_container.end())
        m_container.erase(it);
}

bool SegmentContainer::find(address_t address, RDSegment* segment) const
{
    auto it = std::find_if(m_container.begin(), m_container.end(), [address](const RDSegment& segment) {
        return SegmentContainer::containsAddress(&segment, address);
    });

    if(it == m_container.end()) return false;
    if(segment) *segment = *it;
    return true;
}

bool SegmentContainer::findOffset(offset_t address, RDSegment* segment) const
{
    auto it = std::find_if(m_container.begin(), m_container.end(), [address](const RDSegment& segment) {
        return SegmentContainer::containsOffset(&segment, address);
    });

    if(it == m_container.end()) return false;
    if(segment) *segment = *it;
    return true;
}

size_t SegmentContainer::addressSize(const RDSegment& segment)
{
    return (segment.address > segment.endaddress) ? 0 : (segment.endaddress - segment.address);
}

size_t SegmentContainer::offsetSize(const RDSegment& segment)
{
    if(HAS_FLAG(&segment, SegmentFlags_Bss)) return 0;
    return (segment.offset > segment.endoffset) ? 0 : (segment.endoffset - segment.offset);
}

bool SegmentContainer::containsAddress(const RDSegment* segment, address_t address)
{
    return (address >= segment->address) && (address < segment->endaddress);
}

bool SegmentContainer::containsOffset(const RDSegment* segment, offset_t offset)
{
    if(HAS_FLAG(segment, SegmentFlags_Bss)) return false;
    return (offset >= segment->offset) && (offset < segment->endoffset);
}
