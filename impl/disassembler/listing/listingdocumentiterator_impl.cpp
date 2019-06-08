#include "listingdocumentiterator_impl.h"

namespace REDasm {

ListingDocumentIteratorImpl::ListingDocumentIteratorImpl(ListingDocument &document): m_document(document), m_s_lock(s_lock_safe_ptr(document))
{
    m_iterator = m_s_lock->pimpl_p()->begin();
    m_index = m_s_lock->empty() ? REDasm::npos : 0;
    this->updateCurrent();
}

ListingDocumentIteratorImpl::ListingDocumentIteratorImpl(ListingDocument &document, address_t address, ListingItemType type): m_document(document), m_s_lock(s_lock_safe_ptr(document))
{
    m_iterator = m_s_lock->pimpl_p()->findIterator(address, type);

    size_t idx = std::distance(m_s_lock->pimpl_p()->cbegin(), m_iterator);
    m_index = (idx >= m_s_lock->size()) ? REDasm::npos : idx;

    this->updateCurrent();
}

size_t ListingDocumentIteratorImpl::index() const { return m_index; }

const ListingItem *ListingDocumentIteratorImpl::next()
{
    const ListingItem* item = m_current;
    m_iterator++;
    m_index++;
    this->updateCurrent();
    return item;
}

bool ListingDocumentIteratorImpl::hasNext() const { return m_current; }

void ListingDocumentIteratorImpl::updateCurrent()
{
    if(m_iterator != m_s_lock->pimpl_p()->end())
    {
        m_current = m_iterator->get();
        return;
    }

    m_current = nullptr;
    m_index = REDasm::npos;
}

} // namespace REDasm
