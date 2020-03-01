#pragma once

#include <redasm/disassembler/listing/backend/blocks/blockitem.h>
#include <redasm/disassembler/listing/backend/blocks/blockcontainer.h>
#include <deque>

namespace REDasm {

class BlockContainerImpl
{
    public:
        PIMPL_DECLARE_Q(BlockContainer)
        PIMPL_DECLARE_PUBLIC(BlockContainer)

    public:
        typedef std::deque<BlockItem> Container;

    public:
        BlockContainerImpl(BlockContainer* q);
        void unexplored(const BlockItem* blockitem);
        BlockItem* unexplored(address_t start);
        BlockItem* unexplored(address_t start, address_t end);
        BlockItem* data(address_t start, address_t end, flag_t flags);
        BlockItem* code(address_t start, address_t end, flag_t flags);
        BlockItem* unexploredSize(address_t start, size_t size);
        BlockItem* dataSize(address_t start, size_t size, flag_t flags);
        BlockItem* codeSize(address_t start, size_t size, flag_t flags);

    public:
        bool empty() const;
        size_t size() const;
        size_t indexOf(const BlockItem* bi) const;
        const BlockItem* at(size_t idx) const;
        const BlockItem* find(address_t address) const;
        BlockItem* at(size_t idx);

    private:
        BlockItem *mark(address_t start, address_t end, type_t type, flag_t flags);
        BlockItem *markSize(address_t start, size_t size, type_t type, flag_t flags);
        void remove(address_t start, address_t end);
        BlockItem *insert(address_t start, address_t end, type_t type, flag_t flags);
        Container::iterator findOverlap(address_t address);
        Container::iterator insertionPoint(address_t address);
        template<typename Iterator> Iterator eraseRange(Iterator startit, Iterator endit);
        template<typename Iterator> Iterator eraseBlock(Iterator it);
        template<typename Iterator> Iterator insertBlock(Iterator it, const BlockItem& bi);

    private:
        Container m_blocks;
};

} // namespace REDasm
