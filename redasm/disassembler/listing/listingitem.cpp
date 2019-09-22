#include "listingitem.h"
#include <impl/disassembler/listing/listingitem_impl.h>
#include <impl/libs/cereal/archives/binary.hpp>

namespace REDasm {

ListingItem::ListingItem(): address_new(0), type_new(ListingItemType::Undefined), index_new(0) { }
ListingItem::ListingItem(address_t address, ListingItemType type): address_new(address), type_new(type), index_new(0) { }
ListingItem::ListingItem(address_t address, ListingItemType type, size_t index): address_new(address), type_new(type), index_new(index) { }
bool ListingItem::is(ListingItemType t) const { return type_new == t; }
bool ListingItem::isValid() const { return type_new != ListingItemType::Undefined; }
ListingItemData *ListingItem::data() { return nullptr; }
void ListingItem::save(cereal::BinaryOutputArchive &a) const { a(address_new, type_new, index_new); }
void ListingItem::load(cereal::BinaryInputArchive &a) { a(address_new, type_new, index_new); }

} // namespace REDasm
