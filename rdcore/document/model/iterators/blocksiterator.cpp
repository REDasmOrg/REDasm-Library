#include "blocksiterator.h"
#include "../addressspace.h"

BlocksIterator::BlocksIterator(const AddressSpace* addressspace): m_addressspace(addressspace) { if(!m_addressspace->empty()) this->find(m_addressspace->firstAddress()); }
BlocksIterator::BlocksIterator(const AddressSpace* addressspace, rd_address address): m_addressspace(addressspace) { if(!m_addressspace->empty()) this->find(address); }
std::optional<RDBlock> BlocksIterator::current() const { return m_currblock; }

std::optional<RDBlock> BlocksIterator::find(rd_address address)
{
    auto* bc = m_addressspace->findBlocks(address);
    if(!bc) return std::nullopt;

    if(auto it = bc->find(address); it != bc->end())
    {
        this->seekTo(*it, bc);
        return m_currblock;
    }

    return std::nullopt;
}

std::optional<RDBlock> BlocksIterator::prev()
{
    if(m_segmentidx == RD_NVAL || !m_blockcontainer || m_blockcontainer->empty() || !m_currblock) return std::nullopt;

    if(m_blockcontainer->front() == *m_currblock)
    {
        if(this->seekSegment(-1)) return m_currblock;
        else return std::nullopt;
    }

    auto it = m_blockcontainer->find(*m_currblock);
    if(it == m_blockcontainer->end()) return std::nullopt;
    this->seekTo(*(--it), nullptr);
    return m_currblock;
}

std::optional<RDBlock> BlocksIterator::next()
{
    if(m_segmentidx == RD_NVAL || !m_blockcontainer || m_blockcontainer->empty() || !m_currblock) return std::nullopt;

    if(m_blockcontainer->back() == *m_currblock)
    {
        if(this->seekSegment(1)) return m_currblock;
        else return std::nullopt;
    }

    auto it = m_blockcontainer->find(*m_currblock);
    if(it == m_blockcontainer->end()) return std::nullopt;
    this->seekTo(*(++it), nullptr);
    return m_currblock;
}

void BlocksIterator::seekTo(const RDBlock& b, const BlockContainer* bc)
{
    if(bc) m_blockcontainer = bc;
    m_segmentidx = m_addressspace->indexOfSegment(b.address);
    m_currblock = b;
}

bool BlocksIterator::seekSegment(int dir)
{
    if(!dir) return false;

    size_t i = m_segmentidx;
    const BlockContainer* bc = nullptr;
    RDSegment segment;

    do
    {
        if(!m_addressspace->indexToSegment((dir > 0 ? ++i : --i), &segment)) return false;
        bc = m_addressspace->findBlocks(segment.address);
    }
    while(bc && bc->empty());

    if(!bc) return false;
    this->seekTo(dir > 0 ? bc->front() : bc->back(), bc);
    return true;
}
