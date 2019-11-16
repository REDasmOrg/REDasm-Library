#include "listingitem.h"
#include <impl/disassembler/listing/document/listingitem_impl.h>
#include <impl/libs/cereal/archives/binary.hpp>

#define RETURN_CASE_TYPE_OF(type) case ListingItemType::type: return #type;
#define R_CMP_LISTING_ITEM(t1, t2, op) if(t1->address_new == t2.address_new) { \
                                           if(t1->type_new == t2.type_new) return t1->index_new op t2.index_new; \
                                           return t1->type_new op t2.type_new; \
                                       } \
                                       return t1->address_new op t2.address_new;

namespace REDasm {

ListingItem::ListingItem(): address_new(0), type_new(ListingItemType::None), index_new(0) { }
ListingItem::ListingItem(address_t address, ListingItemType type): address_new(address), type_new(type), index_new(0) { }
ListingItem::ListingItem(address_t address, ListingItemType type, size_t index): address_new(address), type_new(type), index_new(index) { }
bool ListingItem::is(ListingItemType t) const { return type_new == t; }
bool ListingItem::isValid() const { return type_new != ListingItemType::None; }
ListingItemData *ListingItem::data() { return nullptr; }
void ListingItem::save(cereal::BinaryOutputArchive &a) const { a(address_new, type_new, index_new); }
void ListingItem::load(cereal::BinaryInputArchive &a) { a(address_new, type_new, index_new); }
String ListingItem::displayType() { return ListingItem::displayType(this->type_new); }

bool ListingItem::operator ==(const ListingItem& rhs) const
{
    return std::tie(this->address_new, this->type_new, this->index_new) ==
           std::tie(rhs.address_new, rhs.type_new, rhs.index_new);
}

bool ListingItem::operator !=(const ListingItem& rhs) const
{
    return std::tie(this->address_new, this->type_new, this->index_new) !=
           std::tie(rhs.address_new, rhs.type_new, rhs.index_new);
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
