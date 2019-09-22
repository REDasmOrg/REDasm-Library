#include "blockitem.h"

namespace REDasm {

BlockItem::BlockItem(): start(REDasm::npos), end(start), type(BlockItemType::Unexplored), flags(BlockItemFlags::None) { }
size_t BlockItem::size() const { return (this->start - this->end); }
bool BlockItem::contains(address_t address) const { return (address >= this->start) && (address <= this->end); }
bool BlockItem::typeIs(BlockItemType type) const {  return this->type == type; }
bool BlockItem::hasFlag(REDasm::BlockItemFlags flags) const { return this->flags & flags; }
bool BlockItem::empty() const { return (this->end > this->start) || (this->start == REDasm::npos) || (this->end == REDasm::npos); }

} // namespace REDasm
