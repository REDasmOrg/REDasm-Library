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

void Serializer<ListingItem>::write(std::fstream& fs, const ListingItem& d) {
    Serializer<ListingItemData>::write(fs, *d.data());
    Serializer<address_t>::write(fs, d.pimpl_p()->m_address);
    Serializer<ListingItemType>::write(fs, d.pimpl_p()->m_type);
    Serializer<size_t>::write(fs, d.pimpl_p()->m_index);
}

void Serializer<ListingItem>::read(std::fstream& fs, ListingItem& d) {
    Serializer<ListingItemData>::read(fs, *d.data());
    Serializer<address_t>::read(fs, d.pimpl_p()->m_address);
    Serializer<ListingItemType>::read(fs, d.pimpl_p()->m_type);
    Serializer<size_t>::read(fs, d.pimpl_p()->m_index);
}

} // namespace REDasm
