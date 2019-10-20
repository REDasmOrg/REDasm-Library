#pragma once

#include "blockitem.h"
#include "../../../support/event.h"
#include "../../../macros.h"
#include "../../../pimpl.h"

namespace REDasm {

class BlockContainerImpl;

class LIBREDASM_API BlockContainer
{
    PIMPL_DECLARE_P(BlockContainer)
    PIMPL_DECLARE_PRIVATE(BlockContainer)

    public:
        Event inserted;
        Event erased;

    public:
        BlockContainer();
        void unexplored(const BlockItem* blockitem);
        const BlockItem* first() const;
        const BlockItem* last() const;
        BlockItem* unexplored(address_t start);
        BlockItem* unexplored(address_t start, address_t end);
        BlockItem* data(address_t start, address_t end, BlockItemFlags flags = BlockItemFlags::None);
        BlockItem* code(address_t start, address_t end, BlockItemFlags flags = BlockItemFlags::None);
        BlockItem* unexploredSize(address_t start, size_t size);
        BlockItem* dataSize(address_t start, size_t size, BlockItemFlags flags = BlockItemFlags::None);
        BlockItem* codeSize(address_t start, size_t size, BlockItemFlags flags = BlockItemFlags::None);

    public:
        bool empty() const;
        size_t size() const;
        size_t indexOf(const BlockItem* bi) const;
        const BlockItem* at(size_t idx) const;
        const BlockItem* find(address_t address) const;
        BlockItem* at(size_t idx);
};

} // namespace REDasm
