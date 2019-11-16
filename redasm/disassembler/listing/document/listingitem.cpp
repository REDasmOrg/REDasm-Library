#include "listingitem.h"
#include <impl/disassembler/listing/document/listingitem_impl.h>
#include <impl/libs/cereal/archives/binary.hpp>

#define RETURN_CASE_TYPE_OF(type) case ListingItemType::type: return #type;
#define R_CMP_LISTING_ITEM(t1, t2, op) if(t1->address == t2.address) { \
                                           if(t1->type == t2.type) return t1->index op t2.index; \
                                           return t1->type op t2.type; \
                                       } \
                                       return t1->address op t2.address;

namespace REDasm {

ListingItem::ListingItem(): address(0), type(ListingItemType::None), index(0) { }
ListingItem::ListingItem(address_t address, ListingItemType type): address(address), type(type), index(0) { }
ListingItem::ListingItem(address_t address, ListingItemType type, size_t index): address(address), type(type), index(index) { }
bool ListingItem::is(ListingItemType t) const { return type == t; }
bool ListingItem::isValid() const { return type != ListingItemType::None; }
String ListingItem::displayType() { return ListingItem::displayType(this->type); }

bool ListingItem::operator ==(const ListingItem& rhs) const
{
    return std::tie(this->address, this->type, this->index) ==
           std::tie(rhs.address, rhs.type, rhs.index);
}

bool ListingItem::operator !=(const ListingItem& rhs) const
{
    return std::tie(this->address, this->type, this->index) !=
           std::tie(rhs.address, rhs.type, rhs.index);
}

bool ListingItem::operator <(const ListingItem& rhs) const  { R_CMP_LISTING_ITEM(this, rhs, <)  }
bool ListingItem::operator <=(const ListingItem& rhs) const { R_CMP_LISTING_ITEM(this, rhs, <=) }
bool ListingItem::operator >(const ListingItem& rhs) const  { R_CMP_LISTING_ITEM(this, rhs, >)  }
bool ListingItem::operator >=(const ListingItem& rhs) const { R_CMP_LISTING_ITEM(this, rhs, >=) }

String ListingItem::displayType(ListingItemType type)
{
    switch(type)
    {
        RETURN_CASE_TYPE_OF(SegmentItem)
        RETURN_CASE_TYPE_OF(EmptyItem)
        RETURN_CASE_TYPE_OF(FunctionItem)
        RETURN_CASE_TYPE_OF(TypeItem)
        RETURN_CASE_TYPE_OF(SymbolItem)
        RETURN_CASE_TYPE_OF(MetaItem)
        RETURN_CASE_TYPE_OF(InstructionItem)
        RETURN_CASE_TYPE_OF(UnexploredItem)
        RETURN_CASE_TYPE_OF(SeparatorItem)
        default: break;
    }

    return String();
}

} // namespace REDasm
