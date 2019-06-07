#include "listingdocumentiterator.h"
#include <impl/disassembler/listing/listingdocumentiterator_impl.h>

namespace REDasm {

ListingDocumentIterator::ListingDocumentIterator(ListingDocument& document, address_t address, ListingItemType type): m_pimpl_p(new ListingDocumentIteratorImpl(document, address, type)) { }
size_t ListingDocumentIterator::currentIndex() const { PIMPL_P(const ListingDocumentIterator); return p->currentIndex(); }
const ListingItem *ListingDocumentIterator::current() const { PIMPL_P(const ListingDocumentIterator); return p->current(); }
const ListingItem *ListingDocumentIterator::next() { PIMPL_P(ListingDocumentIterator); return p->next(); }
bool ListingDocumentIterator::hasNext() const { PIMPL_P(const ListingDocumentIterator); return p->hasNext(); }

} // namespace REDasm
