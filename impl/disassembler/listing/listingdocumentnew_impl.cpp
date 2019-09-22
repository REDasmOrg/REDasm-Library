#include "listingdocumentnew_impl.h"
#include <redasm/context.h>

namespace REDasm {

ListingDocumentTypeNewImpl::ListingDocumentTypeNewImpl(ListingDocumentTypeNew *q): m_pimpl_q(q) { }
const BlockContainer* ListingDocumentTypeNewImpl::blocks() const { return &m_blocks; }
const ListingItems* ListingDocumentTypeNewImpl::items() const { return &m_items; }
const ListingSegments *ListingDocumentTypeNewImpl::segments() const { return &m_segments; }
const ListingFunctions *ListingDocumentTypeNewImpl::functions() const { return &m_functions; }
const SymbolTable* ListingDocumentTypeNewImpl::symbols() const { return &m_symbols; }
const InstructionCache* ListingDocumentTypeNewImpl::instructions() const { return &m_instructions; }
const ListingCursor& ListingDocumentTypeNewImpl::cursor() const { return m_cursor; }
ListingCursor& ListingDocumentTypeNewImpl::cursor() { return m_cursor; }
const Symbol* ListingDocumentTypeNewImpl::entry() const { return m_entry; }

void ListingDocumentTypeNewImpl::insert(address_t address, ListingItemType item, size_t index)
{
    size_t idx = m_items.insert(address, item, index);
    this->notify(idx, ListingDocumentAction::Inserted);
}

void ListingDocumentTypeNewImpl::function(address_t address, const String& name, SymbolType type, tag_t tag)
{
    if(!m_functions.insert(address))
    {
        r_ctx->log("Function insertion failed @ " + name.hex());
        return;
    }

    m_symbols.create(address, name, type, tag);
    this->insert(address, ListingItemType::FunctionItem);
}

void ListingDocumentTypeNewImpl::notify(size_t idx, ListingDocumentAction action)
{
    if(idx >= m_items.size())
        return;

    PIMPL_Q(ListingDocumentTypeNew);
    ListingDocumentChangedEventArgs e(m_items.at(idx), idx, action);
    q->changed(e);
}

} // namespace REDasm
