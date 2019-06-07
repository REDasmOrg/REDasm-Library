#pragma once

#include <redasm/disassembler/listing/listingdocument.h>
#include <redasm/support/containers/sorted_container.h>
#include <redasm/support/containers/cache_map.h>
#include "listingitem_impl.h"

namespace REDasm {

class ListingDocumentChangedImpl
{
    PIMPL_DECLARE_PUBLIC(ListingDocumentChanged)

    public:
        ListingDocumentChangedImpl(const ListingItem* item, size_t index, ListingDocumentAction action = ListingDocumentAction::Changed);
        const ListingItem* item() const;
        bool isInserted() const;
        bool isRemoved() const;
        size_t index() const;

    private:
        ListingDocumentAction m_action;
        const ListingItem* m_item;
        size_t m_index;
};

class ListingDocumentTypeImpl: public sorted_container<ListingItemPtr, ListingItemPtrComparator>
{
    PIMPL_DECLARE_PUBLIC(ListingDocumentType)

    public:
        typedef sorted_container<ListingItemPtr, ListingItemPtrComparator> ContainerType;

    private:
        typedef cache_map<address_t, InstructionPtr> InstructionCache;
        typedef std::unordered_map<address_t, ListingCommentSet> PendingAutoComments;
        typedef std::unordered_map<address_t, size_t> ActiveMeta;

    private:
        using ContainerType::insert;
        using ContainerType::at;

    public:
        ListingDocumentTypeImpl(ListingDocumentType* q);

    public:
        const_iterator functionStartItem(address_t address) const;
        const_iterator functionItem(address_t address) const;
        const_iterator instructionItem(address_t address) const;
        const_iterator symbolItem(address_t address) const;
        const_iterator segmentItem(address_t address) const;
        const_iterator findItem(address_t address, ListingItemType type, size_t index = 0) const;
        const_iterator findItem(const ListingItem* item) const;
        size_t findIndex(address_t address, ListingItemType type, size_t index = 0) const;
        ListingItem* push(address_t address, ListingItemType type, size_t index = 0);
        void pop(address_t address, ListingItemType type);

    private:
        ListingCursor m_cursor;
        PendingAutoComments m_pendingautocomments;
        SegmentList m_segments;
        ListingFunctions m_functions;
        InstructionCache m_instructions;
        SymbolTable m_symboltable;
        Symbol* m_documententry;
        ActiveMeta m_activemeta;

    friend struct Serializer< safe_ptr<ListingDocumentType> >;
    friend class ListingDocumentIteratorImpl;
    friend class Loader;
};

} // namespace REDasm
