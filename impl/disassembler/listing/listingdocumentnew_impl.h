#pragma once

#include <redasm/disassembler/listing/listingdocumentnew.h>
#include <redasm/disassembler/listing/backend/symboltable.h>
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
        const ListingItem& insert(address_t address, ListingItemType type, size_t index = 0);

    public:
        void notify(size_t idx, ListingDocumentAction action = ListingDocumentAction::Changed);
        void symbol(address_t address, SymbolType type, SymbolFlags flags = SymbolFlags::None, tag_t tag = 0);
        void symbol(address_t address, const String& name, SymbolType type, SymbolFlags flags = SymbolFlags::None, tag_t tag = 0);
        void block(address_t address, size_t size, SymbolType type, SymbolFlags flags = SymbolFlags::None);
        void block(address_t address, size_t size, const String& name, SymbolType type, SymbolFlags flags = SymbolFlags::None);
        void block(const CachedInstruction& instruction);
        void unexplored(address_t address, size_t size);
        bool rename(address_t address, const String& name);

    public:
        const Symbol* symbol(address_t address) const;
        const Symbol* symbol(const String& name) const;
        void remove(address_t address, ListingItemType type);
        void removeAt(size_t idx);

    private:
        bool createSymbol(address_t address, const String& name, SymbolType type, SymbolFlags flags = SymbolFlags::None, tag_t tag = 0);
        bool canOverrideAddress(address_t address, SymbolType type, SymbolFlags flags) const;
        void onBlockInserted(EventArgs* e);
        void onBlockErased(EventArgs* e);

    private:
        ListingCursor m_cursor;
        ListingItems m_items;
        ListingFunctions m_functions;
        ListingSegments m_segments;
        InstructionCache m_instructions;
        BlockContainer m_blocks;
        std::unordered_set<address_t> m_separators;
        std::unordered_map<address_t, ListingItemData> m_itemdata;
        SymbolTable m_symbols;

    private:
        Symbol* m_entry{nullptr};
};

} // namespace REDasm
