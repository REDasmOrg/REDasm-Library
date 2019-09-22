#include "listingitems.h"
#include <impl/disassembler/listing/backend/listingitems_impl.h>

namespace REDasm {

ListingItems::ListingItems(): m_pimpl_p(new ListingItemsImpl()) { }
size_t ListingItems::size() const { PIMPL_P(const ListingItems); return p->size(); }
size_t ListingItems::insert(address_t address, ListingItemType type, size_t index) { return this->insert(ListingItem(address, type, index)); }
size_t ListingItems::insert(const ListingItem& item) { PIMPL_P(ListingItems); return static_cast<size_t>(std::distance(p->begin(), p->insert(item))); }
size_t ListingItems::indexOf(address_t address, ListingItemType type, size_t index) const { ListingItem item(address, type, index); return this->indexOf(item); }
size_t ListingItems::indexOf(const ListingItem& item) const { PIMPL_P(const ListingItems); return p->indexOf(item, ListingItemConstFinderNew()); }
const ListingItem& ListingItems::at(size_t idx) const { PIMPL_P(const ListingItems); return p->at(idx); }
ListingItem& ListingItems::at(size_t idx) { PIMPL_P(ListingItems); return p->at(idx); }
size_t ListingItems::segmentIndex(address_t address, size_t index) const { return this->indexOf(address, ListingItemType::SegmentItem, index); }
size_t ListingItems::functionIndex(address_t address, size_t index) const { return this->indexOf(address, ListingItemType::FunctionItem, index); }
size_t ListingItems::instructionIndex(address_t address, size_t index) const { return this->indexOf(address, ListingItemType::InstructionItem, index); }
size_t ListingItems::symbolIndex(address_t address, size_t index) const { return this->indexOf(address, ListingItemType::SymbolItem, index); }
ListingItem ListingItems::segmentItem(address_t address) const { size_t idx = this->segmentIndex(address); return (idx != REDasm::npos) ? this->at(idx) : ListingItem(); }
ListingItem ListingItems::functionItem(address_t address) const { size_t idx = this->functionIndex(address); return (idx != REDasm::npos) ? this->at(idx) : ListingItem(); }
ListingItem ListingItems::instructionItem(address_t address) const { size_t idx = this->instructionIndex(address); return (idx != REDasm::npos) ? this->at(idx) : ListingItem(); }
ListingItem ListingItems::symbolItem(address_t address) const { size_t idx = this->symbolIndex(address); return (idx != REDasm::npos) ? this->at(idx) : ListingItem(); }

} // namespace REDasm
