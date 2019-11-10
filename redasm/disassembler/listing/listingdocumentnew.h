#pragma once

#include "backend/blocks/blockcontainer.h"
#include "backend/listingfunctions.h"
#include "backend/listingsegments.h"
#include "backend/listingitems.h"
#include "../../support/safe_ptr.h"
#include "listingdocument.h"

namespace REDasm {

class ListingDocumentTypeNewImpl;

class ListingDocumentTypeNew: public Object
{
    REDASM_OBJECT(ListingDocumentTypeNew)
    PIMPL_DECLARE_P(ListingDocumentTypeNew)
    PIMPL_DECLARE_PRIVATE(ListingDocumentTypeNew)

    public:
        ListingDocumentTypeNew();
        const BlockContainer* blocks() const;
        const ListingItems* items() const;
        const ListingSegments* segments() const;
        const ListingFunctions* functions() const;
        const SymbolTable* symbols() const;
        const ListingCursor& cursor() const;
        ListingCursor& cursor();

    public: // Insert
        void entry(address_t address);
        void empty(address_t address);
        bool separator(address_t address);
        void segment(const String& name, offset_t offset, address_t address, u64 size, SegmentType type);
        void segment(const String& name, offset_t offset, address_t address, u64 psize, u64 vsize, SegmentType type);
        void meta(address_t address, const String& s, const String& name = String());
        void type(address_t address, const String& s);
        void table(address_t address, size_t count, tag_t tag = 0);
        void tableItem(address_t address, address_t startaddress, u64 idx, tag_t tag = 0);
        void function(address_t address, const String& name, tag_t tag = 0);
        void function(address_t address, tag_t tag = 0);
        void pointer(address_t address, SymbolType type = SymbolType::Data, tag_t tag = 0);
        void pointer(address_t address, const String& name, SymbolType type = SymbolType::Data, tag_t tag = 0);
        void branch(address_t address, s64 direction, tag_t tag = 0);
        void label(address_t address);
        void data(address_t address, size_t size);
        void data(address_t address, size_t size, const String& name);
        void asciiString(address_t address, size_t size);
        void wideString(address_t address, size_t size);
        void imported(address_t address, size_t size, const String& name);
        void exported(address_t address, const String& name);
        void exportedFunction(address_t address, const String& name);
        void instruction(const CachedInstruction& instruction);

    public: // Count
        size_t blocksCount() const;
        size_t itemsCount() const;
        size_t segmentsCount() const;
        size_t functionsCount() const;
        size_t symbolsCount() const;
        bool empty() const;

    public: // Get-i
        ListingItem& itemAt(size_t idx);
        const ListingItem& itemAt(size_t idx) const;
        const Segment* segmentAt(size_t idx) const;
        address_t functionAt(size_t idx) const;

    public: // Get
        ListingItem currentItem() const;
        CachedInstruction instruction(address_t address);
        address_location function(address_t address) const;
        Segment* segment(address_t address);
        const Segment* segment(address_t address) const;
        const Symbol* symbol(address_t address) const;
        const Symbol* symbol(const String& name) const;
        const BlockItem* block(address_t address) const;
        String type(address_t address) const;
        String comment(address_t address, bool skipauto = false) const;

    public: // Items
        size_t itemIndex(address_t address) const;
        size_t itemListingIndex(address_t address) const;
        size_t itemSegmentIndex(address_t address, size_t index = 0) const;
        size_t itemFunctionIndex(address_t address, size_t index = 0) const;
        size_t itemInstructionIndex(address_t address, size_t index = 0) const;
        size_t itemSymbolIndex(address_t address, size_t index = 0) const;
        ListingItem itemListing(address_t address) const;
        ListingItem itemSegment(address_t address) const;
        ListingItem itemFunction(address_t address) const;
        ListingItem itemInstruction(address_t address) const;
        ListingItem itemSymbol(address_t address) const;

    public: // Item Data
        void autoComment(address_t address, const String& s);
        void comment(address_t address, const String& s);

    public: // Instructions
        CachedInstruction allocateInstruction();
        CachedInstruction entryInstruction();
        size_t entryInstructionIndex() const;
        bool isInstructionCached(address_t address) const;

    public: // Symbols
        bool rename(address_t address, const String& name);
        const Symbol* functionStartSymbol(address_t address) const;
        const Symbol* entry() const;

    public: // Blocks
        const BlockItem* entryBlock() const;
        const BlockItem* blockAt(size_t idx) const;
        const BlockItem* firstBlock() const;
        const BlockItem* lastBlock() const;
        size_t entryBlockIndex() const;

    public: // Graph
        ListingItem functionStart(address_t address) const;
        const FunctionGraph* graph(address_t address) const;
        FunctionGraph* graph(address_t address);
        void graph(address_t address, FunctionGraph* graph);
        void segmentCoverage(address_t address, size_t coverage);
        void segmentCoverageAt(size_t idx, size_t coverage);
        void invalidateGraphs();

    public: // Iteration
        bool next(const BlockItem*& item) const;
        bool next(CachedInstruction& item);

    public:
        void moveToEntry();
        void setEntry(address_t address);
        bool canSymbolizeAddress(address_t address) const;
        bool goTo(const ListingItem& item);
        bool goTo(address_t address);
};

typedef safe_ptr<ListingDocumentTypeNew> ListingDocumentNew;
using document_s_lock_new = s_locked_safe_ptr<ListingDocumentNew>;
using document_x_lock_new = x_locked_safe_ptr<ListingDocumentNew>;

} // namespace REDasm
