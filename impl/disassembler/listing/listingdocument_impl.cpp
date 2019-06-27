#include "listingdocument_impl.h"

namespace REDasm {

ListingDocumentChangedImpl::ListingDocumentChangedImpl(const ListingItem *item, size_t index, ListingDocumentAction action): m_action(action), m_item(item), m_index(index) { }
const ListingItem *ListingDocumentChangedImpl::item() const { return m_item; }
ListingDocumentAction ListingDocumentChangedImpl::action() const { return m_action; }
bool ListingDocumentChangedImpl::isInserted() const { return m_action == ListingDocumentAction::Inserted; }
bool ListingDocumentChangedImpl::isRemoved() const { return m_action == ListingDocumentAction::Removed; }
size_t ListingDocumentChangedImpl::index() const { return m_index; }

ListingDocumentTypeImpl::ListingDocumentTypeImpl(ListingDocumentType *q): sorted_container<ListingItemPtr, ListingItemPtrComparator>(), m_pimpl_q(q), m_documententry(nullptr) { }
ListingDocumentTypeImpl::~ListingDocumentTypeImpl() { m_segments.releaseObjects(); }

ListingDocumentTypeImpl::const_iterator ListingDocumentTypeImpl::functionStartIterator(address_t address) const
{
    PIMPL_Q(const ListingDocumentType);

    const ListingItem* item = q->functionStart(address);
    return item ? this->findIterator(item) : this->end();
}

ListingDocumentTypeImpl::const_iterator ListingDocumentTypeImpl::functionIterator(address_t address) const { return this->findIterator(address, ListingItemType::FunctionItem); }
ListingDocumentTypeImpl::const_iterator ListingDocumentTypeImpl::instructionIterator(address_t address) const { return this->findIterator(address, ListingItemType::InstructionItem); }
ListingDocumentTypeImpl::const_iterator ListingDocumentTypeImpl::symbolIterator(address_t address) const { return this->findIterator(address, ListingItemType::SymbolItem); }
ListingDocumentTypeImpl::const_iterator ListingDocumentTypeImpl::segmentIterator(address_t address) const { return this->findIterator(address, ListingItemType::SegmentItem); }

ListingDocumentTypeImpl::const_iterator ListingDocumentTypeImpl::findIterator(address_t address, ListingItemType type, size_t index) const
{
    auto item = std::make_unique<ListingItem>(address, type, index);
    return this->find(item, ListingItemPtrFinder());
}

ListingDocumentTypeImpl::const_iterator ListingDocumentTypeImpl::findIterator(const ListingItem *item) const { return this->findIterator(item->address(), item->type(), item->index()); }

size_t ListingDocumentTypeImpl::findIndex(address_t address, ListingItemType type, size_t index) const
{
    auto item = std::make_unique<ListingItem>(address, type, index);
    return this->indexOf(item, ListingItemPtrFinder());
}

ListingItem *ListingDocumentTypeImpl::push(address_t address, ListingItemType type, size_t index)
{
    PIMPL_Q(ListingDocumentType);
    auto item = std::unique_ptr<ListingItem>(new ListingItem(address, type, index));

    if(type == ListingItemType::InstructionItem)
    {
        auto it = m_pendingautocomments.find(address);

        if(it != m_pendingautocomments.end())
        {
            item->data()->autocomments = it->second;
            m_pendingautocomments.erase(it);
        }
    }
    else if(type == ListingItemType::FunctionItem)
        m_functions.insert(item.get());

    auto it = ContainerType::find(item);

    if((it != this->end()) && (((*it)->address() == address) && ((*it)->type() == type)))
        return it->get();

    it = ContainerType::insert(std::move(item));
    ListingDocumentChanged ldc(it->get(), std::distance(this->begin(), it), ListingDocumentAction::Inserted);
    q->changed(&ldc);

    return it->get();
}

void ListingDocumentTypeImpl::pop(address_t address, ListingItemType type)
{
    PIMPL_Q(ListingDocumentType);
    ListingItemPtr item = std::make_unique<ListingItem>(address, type, 0);
    auto it = ContainerType::find(item, ListingItemPtrFinder());

    while(it != this->end())
    {
        ListingDocumentChanged ldc(it->get(), std::distance(this->begin(), it), ListingDocumentAction::Removed);
        q->changed(&ldc);

        if(type == ListingItemType::FunctionItem)
            m_functions.erase(it->get());

        this->erase(it);
        it = ContainerType::find(item, ListingItemPtrFinder());
    }
}

} // namespace REDasm
