#pragma once

#include <redasm/disassembler/listing/listingdocumentnew.h>
#include <redasm/disassembler/types/symboltable.h>
#include <unordered_set>
#include "backend/instructioncache.h"
#include "listingitem_impl.h"

namespace REDasm {

class ListingDocumentTypeNewImpl
{
    PIMPL_DECLARE_Q(ListingDocumentTypeNew)
    PIMPL_DECLARE_PUBLIC(ListingDocumentTypeNew)

    public:
        ListingDocumentTypeNewImpl(ListingDocumentTypeNew* q);
        const BlockContainer* blocks() const;
        const ListingItems *items() const;
        const ListingSegments* segments() const;
        const ListingFunctions* functions() const;
        const SymbolTable* symbols() const;
        const InstructionCache* instructions() const;
        const ListingCursor& cursor() const;
        ListingCursor& cursor();
        const Symbol* entry() const;

    public:
        void insert(address_t address, ListingItemType item, size_t index = 0);
        void function(address_t address, const String& name, SymbolType type, tag_t tag = 0);
        void notify(size_t idx, ListingDocumentAction action = ListingDocumentAction::Changed);

    private:
        ListingCursor m_cursor;
        BlockContainer m_blocks;
        ListingItems m_items;
        ListingFunctions m_functions;
        ListingSegments m_segments;
        InstructionCache m_instructions;
        std::unordered_set<address_t> m_separators;
        std::unordered_map<address_t, ListingItemData> m_itemdata;
        SymbolTable m_symbols;

    private:
        Symbol* m_entry{nullptr};
};

} // namespace REDasm
