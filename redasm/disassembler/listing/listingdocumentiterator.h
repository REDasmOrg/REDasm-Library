#pragma once

#include "../../pimpl.h"
#include "listingdocument.h"

namespace REDasm {

class ListingDocumentIteratorImpl;
class ListingDocumentType;

class LIBREDASM_API ListingDocumentIterator
{
    PIMPL_DECLARE_P(ListingDocumentIterator)
    PIMPL_DECLARE_PRIVATE(ListingDocumentIterator)

    public:
        ListingDocumentIterator(ListingDocument &document);
        ListingDocumentIterator(ListingDocument &document, address_t address, ListingItemType type);
        size_t index() const;
        const ListingItem *next();
        bool hasNext() const;
};

} // namespace REDasm
