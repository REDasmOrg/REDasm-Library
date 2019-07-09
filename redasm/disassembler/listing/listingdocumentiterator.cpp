#include "listingdocumentiterator.h"
#include <impl/disassembler/listing/listingdocumentiterator_impl.h>

namespace REDasm {

ListingDocumentIterator::ListingDocumentIterator(ListingDocument &document): m_pimpl_p(new ListingDocumentIteratorImpl(document)) { }
ListingDocumentIterator::ListingDocumentIterator(ListingDocument& document, address_t address, ListingItemType type): m_pimpl_p(new ListingDocumentIteratorImpl(document, address, type)) { }
size_t ListingDocumentIterator::index() const { PIMPL_P(const ListingDocumentIterator); return p->index(); }
const ListingItem *ListingDocumentIterator::current() const { PIMPL_P(const ListingDocumentIterator); return p->current(); }
const ListingItem *ListingDocumentIterator::next() { PIMPL_P(ListingDocumentIterator); return p->next(); }
const ListingItem *ListingDocumentIterator::prev() { PIMPL_P(ListingDocumentIterator); return p->prev();  }
bool ListingDocumentIterator::hasNext() const { PIMPL_P(const ListingDocumentIterator); return p->hasNext(); }
bool ListingDocumentIterator::hasPrevious() const { PIMPL_P(const ListingDocumentIterator); return p->hasPrevious(); }

} // namespace REDasm
