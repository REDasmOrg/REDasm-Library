#pragma once

#include "blockitem.h"
#include "../../../macros.h"
#include "../../../pimpl.h"

namespace REDasm {

class BlockContainerImpl;

class LIBREDASM_API BlockContainer
{
    PIMPL_DECLARE_P(BlockContainer)
    PIMPL_DECLARE_PRIVATE(BlockContainer)

    public:
        BlockContainer();
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
        const BlockItem* at(size_t idx) const;
        const BlockItem* find(address_t address) const;
        BlockItem* at(size_t idx);
};

} // namespace REDasm
