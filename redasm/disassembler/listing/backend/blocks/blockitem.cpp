#include "blockitem.h"

namespace REDasm {

BlockItem::BlockItem(): Object(), start(REDasm::npos), end(start), type(BlockItemType::Unexplored), flags(BlockItemFlags::None) { }
size_t BlockItem::size() const { return (this->end - this->start) + 1; }
String BlockItem::displayRange() const { return "[" + String::hex(start) + ", " + String::hex(end) + "]"; }

String BlockItem::displayType() const
{
    switch(this->type)
    {
        case BlockItemType::Code:       return "CODE";
        case BlockItemType::Data:       return "DATA";
        case BlockItemType::Unexplored: return "UNEXPLORED";
    }

    return String();
}

bool BlockItem::contains(address_t address) const { return (address >= this->start) && (address <= this->end); }
bool BlockItem::typeIs(BlockItemType type) const {  return this->type == type; }
bool BlockItem::hasFlag(REDasm::BlockItemFlags flags) const { return this->flags & flags; }
bool BlockItem::empty() const { return (this->start > this->end) || (this->start == REDasm::npos) || (this->end == REDasm::npos); }

} // namespace REDasm
