#pragma once

#include <redasm/disassembler/listing/listingdocument.h>
#include "../../types/containers/templates/sortedlist_template.h"
#include "instructioncache.h"
#include "listingitem_impl.h"

namespace REDasm {

class ListingDocumentChangedEventArgsImpl
{
    PIMPL_DECLARE_Q(ListingDocumentChangedEventArgs)
    PIMPL_DECLARE_PUBLIC(ListingDocumentChangedEventArgs)

    public:
        ListingDocumentChangedEventArgsImpl(const ListingItem* item, size_t index, ListingDocumentAction action = ListingDocumentAction::Changed);
        const ListingItem* item() const;
        ListingDocumentAction action() const;
        bool isInserted() const;
        bool isRemoved() const;
        size_t index() const;

    private:
        ListingDocumentAction m_action;
        const ListingItem* m_item;
        size_t m_index;
};

class ListingDocumentTypeImpl: public SortedListTemplate<ListingItemPtr, ListingItemPtrComparator>
{
    PIMPL_DECLARE_Q(ListingDocumentType)
    PIMPL_DECLARE_PUBLIC(ListingDocumentType)

    public:
        typedef SortedListTemplate<ListingItemPtr, ListingItemPtrComparator> ContainerType;

    private:
        typedef std::unordered_map<address_t, ListingCommentSet> PendingAutoComments;
        typedef std::unordered_map<address_t, size_t> ActiveMeta;

    private:
        using ContainerType::insert;
        using ContainerType::at;

    public:
        ListingDocumentTypeImpl(ListingDocumentType* q);
        ~ListingDocumentTypeImpl();

    public:
        const_iterator functionStartIterator(address_t address) const;
        const_iterator functionIterator(address_t address) const;
        const_iterator instructionIterator(address_t address) const;
        const_iterator symbolIterator(address_t address) const;
        const_iterator segmentIterator(address_t address) const;
        const_iterator findIterator(address_t address, ListingItemType type, size_t index = 0) const;
        const_iterator findIterator(const ListingItem* item) const;
        size_t findIndex(address_t address, ListingItemType type, size_t index = 0) const;
        ListingItem* push(address_t address, ListingItemType type, size_t index = 0);
        void pop(address_t address, ListingItemType type);
        void save(cereal::BinaryOutputArchive &a) const;
        void load(cereal::BinaryInputArchive &a);

    private:
        void saveItems(cereal::BinaryOutputArchive &a) const;
        void loadItems(cereal::BinaryInputArchive &a);

    private:
        ListingCursor m_cursor;
        PendingAutoComments m_pendingautocomments;
        List m_segments;
        ListingFunctions m_functions;
        InstructionCache m_instructions;
        SymbolTable m_symboltable;
        Symbol* m_documententry;
        ActiveMeta m_activemeta;

    friend class Loader;
};

} // namespace REDasm
