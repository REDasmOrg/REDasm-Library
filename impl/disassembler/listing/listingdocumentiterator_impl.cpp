#include "listingdocumentiterator_impl.h"

namespace REDasm {

ListingDocumentIteratorImpl::ListingDocumentIteratorImpl(ListingDocument &document, address_t address, ListingItemType type): m_document(document), m_s_lock(s_lock_safe_ptr(document))
{
    m_iterator = m_s_lock->pimpl_p()->findItem(address, type);
}

size_t ListingDocumentIteratorImpl::currentIndex() const
{
    if(m_iterator == m_s_lock->pimpl_p()->end())
        return REDasm::npos;

    size_t idx = std::distance(m_s_lock->pimpl_p()->cbegin(), m_iterator);

    if(idx >= m_s_lock->size())
        return REDasm::npos;

    return idx;
}

const ListingItem *ListingDocumentIteratorImpl::current() const { return m_current; }

const ListingItem *ListingDocumentIteratorImpl::next()
{
    if(this->hasNext())
    {
        m_current = m_iterator->get();
        m_iterator++;
    }
    else
        m_current = nullptr;

    return m_current;
}

bool ListingDocumentIteratorImpl::hasNext() const { return m_iterator != m_s_lock->pimpl_p()->end(); }

} // namespace REDasm
