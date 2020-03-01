#include "blockcontainer_impl.h"
#include <redasm/support/event/eventmanager.h>
#include <cassert>
#include <algorithm>

namespace REDasm {

BlockContainerImpl::BlockContainerImpl(BlockContainer* q): m_pimpl_q(q) { }
void BlockContainerImpl::unexplored(const BlockItem* blockitem) { this->unexplored(blockitem->start, blockitem->end); }

BlockItem* BlockContainerImpl::unexplored(address_t start)
{
    auto it = this->findOverlap(start);
    return (it != m_blocks.end()) ? this->unexplored(it->start, it->end) : nullptr;
}

BlockItem *BlockContainerImpl::unexplored(address_t start, address_t end) { return this->mark(start, end, BlockItemType::Unexplored, BlockItemFlags::None); }
BlockItem *BlockContainerImpl::data(address_t start, address_t end, flag_t flags) { return this->mark(start, end, BlockItemType::Data, flags); }
BlockItem *BlockContainerImpl::code(address_t start, address_t end, flag_t flags) { return this->mark(start, end, BlockItemType::Code, flags); }
BlockItem *BlockContainerImpl::unexploredSize(address_t start, size_t size) { return this->markSize(start, size, BlockItemType::Unexplored, BlockItemFlags::None); }
BlockItem *BlockContainerImpl::dataSize(address_t start, size_t size, flag_t flags) { return this->markSize(start, size, BlockItemType::Data, flags); }
BlockItem *BlockContainerImpl::codeSize(address_t start, size_t size, flag_t flags) { return this->markSize(start, size, BlockItemType::Code, flags); }
const BlockItem *BlockContainerImpl::at(size_t idx) const { return std::addressof(m_blocks.at(idx)); }
const BlockItem *BlockContainerImpl::find(address_t address) const { auto it = const_cast<BlockContainerImpl*>(this)->findOverlap(address); return (it != m_blocks.end()) ? std::addressof(*it) : nullptr; }
BlockItem* BlockContainerImpl::at(size_t idx) { return std::addressof(m_blocks.at(idx)); }
BlockItem* BlockContainerImpl::mark(address_t start, address_t end, type_t type, flag_t flags) { assert(end >= start); return this->insert(start, end, type, flags); }
BlockItem* BlockContainerImpl::markSize(address_t start, size_t size, type_t type, flag_t flags) { return this->mark(start, start + size - 1, type, flags); }
bool BlockContainerImpl::empty() const { return m_blocks.empty(); }
size_t BlockContainerImpl::size() const { return m_blocks.size(); }

size_t BlockContainerImpl::indexOf(const BlockItem* bi) const
{
    auto it = const_cast<BlockContainerImpl*>(this)->findOverlap(bi->start);
    return (it != m_blocks.end()) ? std::distance(const_cast<BlockContainerImpl*>(this)->m_blocks.begin(), it) : REDasm::npos;
}

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

    if((begit != m_blocks.end()) && (endit != m_blocks.end())) it = this->eraseRange(begit, endit + 1);
    else if(begit != m_blocks.end()) it = this->eraseBlock(begit);
    else if(endit != m_blocks.end()) it = this->eraseBlock(endit);
    else assert(false);

    if(!begbl.empty())
    {
        begbl.type = BlockItemType::Unexplored; // Demote to "Unexplored"
        it = this->insertBlock(it, begbl);
        it++;
    }

    if(!endbl.empty())
    {
        endbl.type = BlockItemType::Unexplored; // Demote to "Unexplored"
        this->insertBlock(it, endbl);
    }
}

BlockItem* BlockContainerImpl::insert(address_t start, address_t end, type_t type, flag_t flags)
{
    this->remove(start, end);
    auto it = this->insertionPoint(start);

    BlockItem bi;
    bi.start = start;
    bi.end = end;
    bi.type = type;
    bi.flags = flags;

    it = this->insertBlock(it, bi);
    return std::addressof(*it);
}

BlockContainerImpl::Container::iterator BlockContainerImpl::findOverlap(address_t address)
{
    Container::iterator first = m_blocks.begin(), it;
    Container::iterator::difference_type count = std::distance(first, m_blocks.end()), step;

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

template<typename Iterator> Iterator BlockContainerImpl::eraseRange(Iterator startit, Iterator endit)
{
    auto it = std::remove_if(startit, endit, [&](BlockItem& bi) -> bool {
       EventManager::trigger<StandardEvents::Document_BlockErased>(&bi);
       return true;
    });

    return m_blocks.erase(it, endit);
}

template<typename Iterator> Iterator BlockContainerImpl::eraseBlock(Iterator it)
{
    BlockItem bi = *it;
    it = m_blocks.erase(it);
    EventManager::trigger<StandardEvents::Document_BlockErased>(&bi);
    return it;
}

template<typename Iterator> Iterator BlockContainerImpl::insertBlock(Iterator it, const BlockItem& bi)
{
    auto resit = m_blocks.insert(it, bi);
    EventManager::trigger<StandardEvents::Document_BlockInserted>(std::addressof(*resit));
    return resit;
}

template BlockContainerImpl::Container::iterator BlockContainerImpl::eraseRange<BlockContainerImpl::Container::iterator>(BlockContainerImpl::Container::iterator, BlockContainerImpl::Container::iterator);
template BlockContainerImpl::Container::iterator BlockContainerImpl::eraseBlock(BlockContainerImpl::Container::iterator);
template BlockContainerImpl::Container::iterator BlockContainerImpl::insertBlock(BlockContainerImpl::Container::iterator, const BlockItem&);

} // namespace REDasm
