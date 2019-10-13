#include "blockcontainer_impl.h"
#include <cassert>

namespace REDasm {

void BlockContainerImpl::unexplored(const BlockItem* blockitem) { this->unexplored(blockitem->start, blockitem->end); }

BlockItem* BlockContainerImpl::unexplored(address_t start)
{
    auto it = this->findOverlap(start);
    return (it != m_blocks.end()) ? this->unexplored(it->start, it->end) : nullptr;
}

BlockItem *BlockContainerImpl::unexplored(address_t start, address_t end) { return this->mark(start, end, BlockItemType::Unexplored, BlockItemFlags::None); }
BlockItem *BlockContainerImpl::data(address_t start, address_t end, BlockItemFlags flags) { return this->mark(start, end, BlockItemType::Data, flags); }
BlockItem *BlockContainerImpl::code(address_t start, address_t end, BlockItemFlags flags) { return this->mark(start, end, BlockItemType::Code, flags); }
BlockItem *BlockContainerImpl::unexploredSize(address_t start, size_t size) { return this->markSize(start, size, BlockItemType::Unexplored, BlockItemFlags::None); }
BlockItem *BlockContainerImpl::dataSize(address_t start, size_t size, BlockItemFlags flags) { return this->markSize(start, size, BlockItemType::Data, flags); }
BlockItem *BlockContainerImpl::codeSize(address_t start, size_t size, BlockItemFlags flags) { return this->markSize(start, size, BlockItemType::Code, flags); }
const BlockItem *BlockContainerImpl::at(size_t idx) const { return &(m_blocks.at(idx)); }
const BlockItem *BlockContainerImpl::find(address_t address) const { auto it = this->findOverlap(address); return (it != m_blocks.end()) ? &(*it) : nullptr; }
BlockItem* BlockContainerImpl::at(size_t idx) { return &(m_blocks.at(idx)); }
BlockItem* BlockContainerImpl::mark(address_t start, address_t end, BlockItemType type, BlockItemFlags flags) { assert(end >= start); return this->insert(start, end, type, flags); }
BlockItem* BlockContainerImpl::markSize(address_t start, size_t size, BlockItemType type, BlockItemFlags flags) { return this->mark(start, start + size - 1, type, flags); }
bool BlockContainerImpl::empty() const { return m_blocks.empty(); }
size_t BlockContainerImpl::size() const { return m_blocks.size(); }

void BlockContainerImpl::remove(address_t start, address_t end)
{
    auto begit = this->findOverlap(start);
    auto endit = this->findOverlap(end);

    if((begit == m_blocks.end()) && (endit == m_blocks.end()))
        return;

    BlockItem begbl, endbl;

    if(begit != m_blocks.end())
    {
        begbl = *begit;
        begbl.end = start - 1; // Shrink first part
    }

    if(endit != m_blocks.end())
    {
        endbl = *endit;
        endbl.start = end + 1; // Shrink last part
    }

    auto it = m_blocks.end();

    if((begit != m_blocks.end()) && (endit != m_blocks.end())) it = m_blocks.erase(begit, endit + 1);
    else if(begit != m_blocks.end()) it = m_blocks.erase(begit);
    else if(endit != m_blocks.end()) it = m_blocks.erase(endit);
    else assert(false);

    if(!begbl.empty())
    {
        it = m_blocks.insert(it, begbl);
        it++;
    }

    if(!endbl.empty())
        m_blocks.insert(it, endbl);
}

BlockItem* BlockContainerImpl::insert(address_t start, address_t end, BlockItemType type, BlockItemFlags flags)
{
    this->remove(start, end);
    auto it = this->insertionPoint(start);

    if((it != m_blocks.end()) && (type == BlockItemType::Unexplored) && it->typeIs(type))
    {
        if(start < it->start) it->start = start;
        if(end > it->end) it->end = end;
        return &(*it);
    }

    BlockItem bi;
    bi.start = start;
    bi.end = end;
    bi.type = type;
    bi.flags = flags;

    it = m_blocks.insert(it, bi);
    return &(*it);
}

BlockContainerImpl::Container::const_iterator BlockContainerImpl::findOverlap(address_t address) const
{
    Container::const_iterator first = m_blocks.begin(), it;
    Container::const_iterator::difference_type count = std::distance(first, m_blocks.end()), step;

    while(count > 0)
    {
        it = first;
        step = count / 2;
        std::advance(it, step);

        if(it->contains(address))
            return it;

        if(it->start < address)
        {
            first = ++it;
            count -= step + 1;
        }
        else
            count = step;
    }

    return m_blocks.end();
}

BlockContainerImpl::Container::iterator BlockContainerImpl::insertionPoint(address_t address)
{
    auto first = m_blocks.begin(), last = m_blocks.end();
    auto count = std::distance(first,last);

    while(count > 0)
    {
        auto it = first;
        auto step = count / 2;
        std::advance(it, step);

        if(it->start < address)
        {
            first = ++it;
            count -= step + 1;
        }
        else
            count = step;
    }

    return first;
}

} // namespace REDasm
