#pragma once

#include <unordered_map>
#include <unordered_set>
#include <redasm/disassembler/listing/backend/symbols/symboltable.h>
#include <redasm/disassembler/listing/document/listingdocument.h>
#include "../backend/instructioncache.h"
#include "listingitem_impl.h"

namespace REDasm {

class ListingDocumentTypeImpl
{
    PIMPL_DECLARE_Q(ListingDocumentType)
    PIMPL_DECLARE_PUBLIC(ListingDocumentType)

    public:
        ListingDocumentTypeImpl(ListingDocumentType* q);
        ~ListingDocumentTypeImpl();
        const BlockContainer* blocks() const;
        const ListingItems *items() const;
        const ListingSegments* segments() const;
        const ListingFunctions* functions() const;
        const SymbolTable* symbols() const;
        const InstructionCache* instructions() const;
        const ListingCursor& cursor() const;
        ListingCursor& cursor();

    public:
        ListingItem functionStart(address_t address) const;
        const ListingItem& insert(address_t address, type_t type, size_t index = 0);
        void notify(size_t idx, size_t action = ListingDocumentChangedEventArgs::Changed);
        void symbol(address_t address, type_t type, flag_t flags = Symbol::T_None, tag_t tag = 0);
        void symbol(address_t address, const String& name, type_t type, flag_t flags = Symbol::T_None, tag_t tag = 0);
        void block(address_t address, size_t size, type_t type, flag_t flags = Symbol::T_None, tag_t tag = 0);
        void block(address_t address, size_t size, const String& name, type_t type, flag_t flags = Symbol::T_None, tag_t tag = 0);
        void block(const CachedInstruction& instruction);
        void unexplored(address_t address, size_t size);
        bool rename(address_t address, const String& name);

    public:
        const Symbol* symbol(address_t address) const;
        const Symbol* symbol(const String& name) const;
        void replace(address_t address, type_t type);
        void remove(address_t address, type_t type);
        void removeAt(size_t idx);
        const FunctionGraph* graph(address_t address) const;
        FunctionGraph* graph(address_t address);
        void graph(address_t address, FunctionGraph* graph);
        void segmentCoverage(address_t address, size_t coverage);
        void segmentCoverageAt(size_t idx, size_t coverage);
        void invalidateGraphs();

    private:
        void createSymbol(address_t address, const String& name, type_t type, flag_t flags = Symbol::T_None, tag_t tag = 0);
        bool canSymbolizeAddress(address_t address, type_t type, flag_t flags) const;
        void onBlockInserted(const EventArgs* e);
        void onBlockErased(const EventArgs* e);

    private:
        ListingCursor m_cursor;
        ListingItems m_items;
        ListingFunctions m_functions;
        ListingSegments m_segments;
        InstructionCache m_instructions;
        BlockContainer m_blocks;
        std::unordered_set<address_t> m_separators;
        std::unordered_map<address_t, size_t> m_metas;
        std::unordered_map<address_t, ListingItemData> m_itemdata;
        SymbolTable m_symbols;

    private:
        Symbol* m_entry{nullptr};
};

} // namespace REDasm
