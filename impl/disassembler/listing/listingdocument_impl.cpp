#include "listingdocument_impl.h"
#include "../../impl/libs/cereal/archives/binary.hpp"
#include <redasm/context.h>

namespace REDasm {

ListingDocumentChangedImpl::ListingDocumentChangedImpl(const ListingItem *item, size_t index, ListingDocumentAction action): m_action(action), m_item(item), m_index(index) { }
const ListingItem *ListingDocumentChangedImpl::item() const { return m_item; }
ListingDocumentAction ListingDocumentChangedImpl::action() const { return m_action; }
bool ListingDocumentChangedImpl::isInserted() const { return m_action == ListingDocumentAction::Inserted; }
bool ListingDocumentChangedImpl::isRemoved() const { return m_action == ListingDocumentAction::Removed; }
size_t ListingDocumentChangedImpl::index() const { return m_index; }

ListingDocumentTypeImpl::ListingDocumentTypeImpl(ListingDocumentType *q): SortedListTemplate<ListingItemPtr, ListingItemPtrComparator>(), m_pimpl_q(q), m_documententry(nullptr) { }
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
            m_functions.remove(it->get());

        this->erase(it);
        it = ContainerType::find(item, ListingItemPtrFinder());
    }
}

void ListingDocumentTypeImpl::save(cereal::BinaryOutputArchive &a) const
{
    address_t entry = m_documententry ? m_documententry->address : 0;

    a(entry, m_segments);
    m_symboltable.save(a);
    this->saveItems(a);
    m_cursor.save(a);
}

void ListingDocumentTypeImpl::load(cereal::BinaryInputArchive &a)
{
    address_t entry = 0;

    a(entry, m_segments);
    m_symboltable.load(a);
    this->loadItems(a);
    m_cursor.load(a);

    m_documententry = m_symboltable.symbol(entry);
}

void ListingDocumentTypeImpl::saveItems(cereal::BinaryOutputArchive &a) const
{
    size_t size = this->size();
    a(size);

    for(auto it = this->begin(); it != this->end(); it++)
        it->get()->save(a);
}

void ListingDocumentTypeImpl::loadItems(cereal::BinaryInputArchive &a)
{
    size_t size = 0;
    a(size);

    for(size_t i = 0; i < size; i++)
    {
        auto item = std::make_unique<ListingItem>();
        item->load(a);

        if(item->is(ListingItemType::InstructionItem))
        {
            PIMPL_Q(ListingDocumentType);
            q->instruction(r_disasm->disassembleInstruction(item->address()));
        }

        this->insert(std::move(item));
    }
}

} // namespace REDasm
