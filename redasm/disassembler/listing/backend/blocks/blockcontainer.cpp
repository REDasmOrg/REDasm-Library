#include "blockcontainer.h"
#include <impl/disassembler/listing/backend/blocks/blockcontainer_impl.h>

namespace REDasm {

BlockContainer::BlockContainer(): m_pimpl_p(new BlockContainerImpl(this)) { }
void BlockContainer::unexplored(const BlockItem* blockitem) { PIMPL_P(BlockContainer); return p->unexplored(blockitem);  }
const BlockItem* BlockContainer::first() const { PIMPL_P(const BlockContainer); return !p->m_blocks.empty() ? &p->m_blocks.front() : nullptr; }
const BlockItem* BlockContainer::last() const { PIMPL_P(const BlockContainer);  return !p->m_blocks.empty() ? &p->m_blocks.back() : nullptr; }
BlockItem *BlockContainer::unexplored(address_t start) { PIMPL_P(BlockContainer); return p->unexplored(start); }
BlockItem *BlockContainer::unexplored(address_t start, address_t end) { PIMPL_P(BlockContainer); return p->unexplored(start, end); }
BlockItem *BlockContainer::data(address_t start, address_t end, flag_t flags) { PIMPL_P(BlockContainer); return p->data(start, end, flags); }
BlockItem *BlockContainer::code(address_t start, address_t end, flag_t flags) { PIMPL_P(BlockContainer); return p->code(start, end, flags); }
BlockItem *BlockContainer::unexploredSize(address_t start, size_t size) { PIMPL_P(BlockContainer); return p->unexploredSize(start, size); }
BlockItem *BlockContainer::dataSize(address_t start, size_t size, flag_t flags) { PIMPL_P(BlockContainer); return p->dataSize(start, size, flags); }
BlockItem *BlockContainer::codeSize(address_t start, size_t size, flag_t flags) { PIMPL_P(BlockContainer); return p->codeSize(start, size, flags); }
bool BlockContainer::empty() const { PIMPL_P(const BlockContainer); return p->empty(); }
size_t BlockContainer::size() const { PIMPL_P(const BlockContainer); return p->size(); }
size_t BlockContainer::indexOf(const BlockItem* bi) const { PIMPL_P(const BlockContainer); return p->indexOf(bi); }
const BlockItem *BlockContainer::at(size_t idx) const { PIMPL_P(const BlockContainer); return p->at(idx); }
const BlockItem *BlockContainer::find(address_t address) const { PIMPL_P(const BlockContainer); return p->find(address); }
BlockItem *BlockContainer::at(size_t idx) { PIMPL_P(BlockContainer); return p->at(idx); }

} // namespace REDasm
