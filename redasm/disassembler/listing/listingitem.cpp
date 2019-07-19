#include "listingitem.h"
#include <impl/disassembler/listing/listingitem_impl.h>

namespace REDasm {

ListingItem::ListingItem(): m_pimpl_p(new ListingItemImpl()) { }
ListingItem::ListingItem(address_t address, ListingItemType type, size_t index): m_pimpl_p(new ListingItemImpl(address, type, index)) { }
bool ListingItem::is(ListingItemType t) const { PIMPL_P(const ListingItem); return p->m_type == t; }
address_t ListingItem::address() const { PIMPL_P(const ListingItem); return p->m_address; }
ListingItemType ListingItem::type() const { PIMPL_P(const ListingItem); return p->m_type; }
size_t ListingItem::index() const { PIMPL_P(const ListingItem); return p->m_index; }
ListingItemData *ListingItem::data() const { PIMPL_P(const ListingItem); return p->m_data.get();  }
void ListingItem::save(cereal::BinaryOutputArchive &a) const { PIMPL_P(const ListingItem); p->save(a); }
void ListingItem::load(cereal::BinaryInputArchive &a) { PIMPL_P(ListingItem); p->load(a); }

} // namespace REDasm
