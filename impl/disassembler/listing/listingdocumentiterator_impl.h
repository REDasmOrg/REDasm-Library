#pragma once

#include <redasm/disassembler/listing/listingdocumentiterator.h>
#include "listingdocument_impl.h"

namespace REDasm {

class ListingDocumentIteratorImpl
{
    public:
        ListingDocumentIteratorImpl(ListingDocument &document, address_t address, ListingItemType type);
        size_t currentIndex() const;
        const ListingItem* current() const;
        const ListingItem* next();
        bool hasNext() const;

    private:
        ListingDocumentTypeImpl::const_iterator m_iterator;
        const ListingDocument& m_document;
        const ListingItem* m_current;
        document_s_lock m_s_lock;
};

} // namespace REDasm
