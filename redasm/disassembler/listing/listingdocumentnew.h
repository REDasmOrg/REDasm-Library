#pragma once

#include "backend/blocks/blockcontainer.h"
#include "backend/listingfunctions.h"
#include "backend/listingsegments.h"
#include "backend/listingitems.h"
#include "../../support/safe_ptr.h"
#include "listingdocument.h"

namespace REDasm {

class ListingDocumentTypeNewImpl;

class ListingDocumentTypeNew
{
    PIMPL_DECLARE_P(ListingDocumentTypeNew)
    PIMPL_DECLARE_PRIVATE(ListingDocumentTypeNew)

    public:
        Event changed;

    public:
        ListingDocumentTypeNew();
        const BlockContainer* blocks() const;
        const ListingItems* items() const;
        const ListingSegments* segments() const;
        const ListingFunctions* functions() const;
        const SymbolTable* symbols() const;
        const ListingCursor& cursor() const;
        ListingCursor& cursor();
        const Symbol* entry() const;

    public: // Insert
        void entry(address_t address);
        void empty(address_t address);
        bool separator(address_t address);
        void segment(const String& name, offset_t offset, address_t address, u64 size, SegmentType type);
        void segment(const String& name, offset_t offset, address_t address, u64 psize, u64 vsize, SegmentType type);
        void function(address_t address, const String& name, tag_t tag = 0);
        void function(address_t address, tag_t tag = 0);
        void pointer(address_t address, SymbolType type = SymbolType::Data, tag_t tag = 0);
        void branch(address_t address, s64 direction, tag_t tag = 0);
        void label(address_t address, tag_t tag = 0);

    public: // Blocks
        void asciiString(address_t address, size_t len);
        void wideString(address_t address, size_t len);
        void instruction(const CachedInstruction& instruction);

    public: // Count
        size_t blocksCount() const;
        size_t itemsCount() const;
        size_t segmentsCount() const;
        size_t functionsCount() const;
        size_t symbolsCount() const;

    public: // Get-i
        ListingItem itemAt(size_t idx);
        const Segment* segmentAt(size_t idx);

    public: // Get
        CachedInstruction instruction(address_t address);
        address_location function(address_t address) const;
        const Segment* segment(address_t address) const;
        const Symbol* symbol(address_t address) const;
        const Symbol* symbol(const String& name) const;

    public: // Items
        size_t itemSegmentIndex(address_t address, size_t index = 0) const;
        size_t itemFunctionIndex(address_t address, size_t index = 0) const;
        size_t itemInstructionIndex(address_t address, size_t index = 0) const;
        size_t itemSymbolIndex(address_t address, size_t index = 0) const;
        ListingItem itemSegment(address_t address) const;
        ListingItem itemFunction(address_t address) const;
        ListingItem itemInstruction(address_t address) const;
        ListingItem itemSymbol(address_t address) const;

    public: // Item Data
        void autoComment(address_t address, const String& s);

    public: // Instructions
        CachedInstruction cacheInstruction(address_t address);
        bool isInstructionCached(address_t address) const;

    public:
        void moveToEntry();
        void setEntry(address_t address);
};

typedef safe_ptr<ListingDocumentTypeNew> ListingDocumentNew;
using document_s_lock_new = s_locked_safe_ptr<ListingDocumentNew>;
using document_x_lock_new = x_locked_safe_ptr<ListingDocumentNew>;

} // namespace REDasm
