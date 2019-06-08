#pragma once

#include <redasm/disassembler/listing/listingdocumentiterator.h>
#include "listingdocument_impl.h"

namespace REDasm {

class ListingDocumentIteratorImpl
{
    public:
        ListingDocumentIteratorImpl(ListingDocument &document);
        ListingDocumentIteratorImpl(ListingDocument &document, address_t address, ListingItemType type);
        size_t index() const;
        const ListingItem* next();
        bool hasNext() const;

    private:
        void updateCurrent();

    private:
        ListingDocumentTypeImpl::const_iterator m_iterator;
        const ListingDocument& m_document;
        const ListingItem* m_current;
        document_s_lock m_s_lock;
        size_t m_index;
};

} // namespace REDasm
