#include "listingitems.h"
#include <impl/disassembler/listing/backend/listingitems_impl.h>

namespace REDasm {

ListingItems::ListingItems(): m_pimpl_p(new ListingItemsImpl()) { }
bool ListingItems::empty() const { PIMPL_P(const ListingItems); return p->empty(); }
size_t ListingItems::size() const { PIMPL_P(const ListingItems); return p->size(); }
size_t ListingItems::insert(address_t address, type_t type, size_t index) { return this->insert(ListingItem(address, type, index)); }
size_t ListingItems::insert(const ListingItem& item) { PIMPL_P(ListingItems); return static_cast<size_t>(std::distance(p->begin(), p->insert(item))); }
size_t ListingItems::indexOf(address_t address, type_t type, size_t index) const { ListingItem item(address, type, index); return this->indexOf(item); }
size_t ListingItems::indexOf(const ListingItem& item) const { PIMPL_P(const ListingItems); return p->indexOf(item, ListingItemConstFinderNew()); }
const ListingItem& ListingItems::at(size_t idx) const { PIMPL_P(const ListingItems); return p->at(idx); }
ListingItem& ListingItems::at(size_t idx) { PIMPL_P(ListingItems); return p->at(idx); }
void ListingItems::erase(const ListingItem& item) { PIMPL_P(ListingItems); p->erase(item); }
void ListingItems::erase(size_t idx) { PIMPL_P(ListingItems); p->eraseAt(idx); }

size_t ListingItems::itemIndex(address_t address) const
{
    for(type_t t = ListingItem::FirstItem; t <= ListingItem::LastItem; t = static_cast<type_t>(static_cast<size_t>(t) + 1))
    {
        size_t idx = this->indexOf(address, t);
        if(idx != REDasm::npos) return idx;
    }

    return REDasm::npos;
}

size_t ListingItems::listingIndex(address_t address) const
{
    size_t idx = this->indexOf(address, ListingItem::SymbolItem);
    if(idx == REDasm::npos) idx = this->indexOf(address, ListingItem::InstructionItem);
    if(idx == REDasm::npos) idx = this->indexOf(address, ListingItem::UnexploredItem);
    return idx;
}

size_t ListingItems::segmentIndex(address_t address, size_t index) const { return this->indexOf(address, ListingItem::SegmentItem, index); }
size_t ListingItems::functionIndex(address_t address, size_t index) const { return this->indexOf(address, ListingItem::FunctionItem, index); }
size_t ListingItems::instructionIndex(address_t address, size_t index) const { return this->indexOf(address, ListingItem::InstructionItem, index); }
size_t ListingItems::symbolIndex(address_t address, size_t index) const { return this->indexOf(address, ListingItem::SymbolItem, index); }
ListingItem ListingItems::listingItem(address_t address) const { size_t idx = this->listingIndex(address); return (idx != REDasm::npos) ? this->at(idx) : ListingItem();  }
ListingItem ListingItems::segmentItem(address_t address) const { size_t idx = this->segmentIndex(address); return (idx != REDasm::npos) ? this->at(idx) : ListingItem(); }
ListingItem ListingItems::functionItem(address_t address) const { size_t idx = this->functionIndex(address); return (idx != REDasm::npos) ? this->at(idx) : ListingItem(); }
ListingItem ListingItems::instructionItem(address_t address) const { size_t idx = this->instructionIndex(address); return (idx != REDasm::npos) ? this->at(idx) : ListingItem(); }
ListingItem ListingItems::symbolItem(address_t address) const { size_t idx = this->symbolIndex(address); return (idx != REDasm::npos) ? this->at(idx) : ListingItem(); }

} // namespace REDasm
