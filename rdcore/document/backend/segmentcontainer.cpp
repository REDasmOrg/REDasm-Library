#include "segmentcontainer.h"
#include "../../config.h"
#include <algorithm>

SegmentContainer::SegmentContainer()
{
    m_oninsert = [](const RDBlock&) { };
    m_onremove = [](const RDBlock&) { };
}

void SegmentContainer::whenInsert(const BlockContainer::Callback& cb) { m_oninsert = cb; }
void SegmentContainer::whenRemove(const BlockContainer::Callback& cb) { m_onremove = cb; }

void SegmentContainer::removeAddress(rd_address address)
{
    auto it = std::find_if(m_container.begin(), m_container.end(), [address](const RDSegment& segment) {
        return SegmentContainer::containsAddress(&segment, address);
    });

    if(it != m_container.end())
        m_container.erase(it);
}

bool SegmentContainer::markCode(rd_address address, size_t size)
{
    auto* blocks = this->findBlocks(address);
    if(!blocks) return false;

    blocks->codeSize(address, size);
    return true;
}

bool SegmentContainer::markData(rd_address address, size_t size)
{
    auto* blocks = this->findBlocks(address);
    if(!blocks) return false;

    blocks->dataSize(address, size);
    return true;
}

bool SegmentContainer::find(rd_address address, RDSegment* segment) const
{
    auto it = std::find_if(m_container.begin(), m_container.end(), [address](const RDSegment& segment) {
        return SegmentContainer::containsAddress(&segment, address);
    });

    if(it == m_container.end()) return false;
    if(segment) *segment = *it;
    return true;
}

bool SegmentContainer::findOffset(rd_offset offset, RDSegment* segment) const
{
    auto it = std::find_if(m_container.begin(), m_container.end(), [offset](const RDSegment& segment) {
        return SegmentContainer::containsOffset(&segment, offset);
    });

    if(it == m_container.end()) return false;
    if(segment) *segment = *it;
    return true;
}

bool SegmentContainer::findBlock(rd_address address, RDBlock* block) const
{
    auto* blocks = this->findBlocks(address);
    if(!blocks) return false;
    return blocks->find(address, block);
}

bool SegmentContainer::setUserData(rd_address address, uintptr_t userdata)
{
    auto it = std::find_if(m_container.begin(), m_container.end(), [address](const RDSegment& segment) {
        return SegmentContainer::containsAddress(&segment, address);
    });

    if(it == m_container.end()) return false;
    it->u_data = userdata;
    return true;
}

size_t SegmentContainer::insert(const RDSegment& segment)
{
    RDSegment s;

    if(this->find(segment.address, &s))
    {
        rd_cfg->log("Segment '" + std::string(segment.name) + "' overlaps '" + s.name + "'");
        return RD_NPOS;
    }

    if(this->find(segment.endaddress - 1, &s))
    {
        rd_cfg->log("Segment '" + std::string(segment.name) + "' overlaps '" + s.name + "'");
        return RD_NPOS;
    }

    size_t res = ClassType::insert(segment);
    auto [it, inserted] = m_blocks.insert({ segment.address, { } });

    if(!inserted)
    {
        rd_cfg->log("Segment insertion failed (" + std::string(segment.name) + ")");
        return RD_NPOS;
    }

    it->second.whenInsert(m_oninsert);
    it->second.whenRemove(m_onremove);
    it->second.unexploredSize(segment.address, SegmentContainer::addressSize(segment));
    return res;
}

BlockContainer* SegmentContainer::findBlocks(rd_address address) const
{
    return const_cast<SegmentContainer*>(this)->findBlocks(address);
}

BlockContainer* SegmentContainer::findBlocks(rd_address address)
{
    for(auto& [startaddress, blocks] : m_blocks)
    {
        if(blocks.contains(address)) return &blocks;
    }

    return nullptr;
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

bool SegmentContainer::containsAddress(const RDSegment* segment, rd_address address)
{
    return (address >= segment->address) && (address < segment->endaddress);
}

bool SegmentContainer::containsOffset(const RDSegment* segment, rd_offset offset)
{
    if(HAS_FLAG(segment, SegmentFlags_Bss)) return false;
    return (offset >= segment->offset) && (offset < segment->endoffset);
}
