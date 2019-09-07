#pragma once

#include "../../support/safe_ptr.h"
#include "../../support/event.h"
#include "../types/symboltable.h"
#include "cachedinstruction.h"
#include "backend/listingfunctions.h"
#include "listingcursor.h"
#include "listingitem.h"

namespace REDasm {

class ListingDocumentChangedEventArgsImpl;
class ListingDocumentTypeImpl;

enum class ListingDocumentAction { Changed = 0, Inserted, Removed };

class LIBREDASM_API ListingDocumentChangedEventArgs: public EventArgs
{
    PIMPL_DECLARE_P(ListingDocumentChangedEventArgs)
    PIMPL_DECLARE_PRIVATE(ListingDocumentChangedEventArgs)

    public:
        ListingDocumentChangedEventArgs(const ListingItem* item, size_t index, ListingDocumentAction action = ListingDocumentAction::Changed);
        const ListingItem* item() const;
        ListingDocumentAction action() const;
        bool isInserted() const;
        bool isRemoved() const;
        size_t index() const;
};

class LIBREDASM_API ListingDocumentType: public Object
{
    REDASM_OBJECT(ListingDocument)
    PIMPL_DECLARE_P(ListingDocumentType)
    PIMPL_DECLARE_PRIVATE(ListingDocumentType)

    public:
        Event changed;

    public:
        void save(cereal::BinaryOutputArchive &a) const override;
        void load(cereal::BinaryInputArchive &a) override;

    public:
        ListingDocumentType();
        ~ListingDocumentType() = default;
        size_t size() const;
        bool empty() const;
        bool advance(CachedInstruction& instruction);
        bool goTo(const ListingItem* item);
        bool goTo(address_t address);
        void moveToEP();

    public: // ListingItemData functions
        String comment(const ListingItem *item, bool skipauto = false) const;
        void comment(const ListingItem* item, const String& s);
        void autoComment(address_t address, const String& s);

    public: // ListingItem getter functions
        String type(const ListingItem *item) const;
        void type(address_t address, const String& s);
        const ListingMetaItem &meta(const ListingItem *item) const;

    public: // ListingItem inserter functions
        void empty(address_t address);
        bool separator(address_t address);
        void meta(address_t address, const String& s, const String& name = String());
        void entry(address_t address, tag_t tag = 0);
        void function(address_t address, const String& name, tag_t tag = 0);
        void function(address_t address, tag_t tag = 0);
        void pointer(address_t address, SymbolType type, tag_t tag = 0);
        void table(address_t address, size_t count, tag_t tag = 0);
        void tableItem(address_t address, address_t startaddress, u64 idx, tag_t tag = 0);
        void branch(address_t address, s64 direction, tag_t tag = 0);
        void symbol(address_t address, const String& name, SymbolType type, tag_t tag = 0);
        void symbol(address_t address, SymbolType type, tag_t tag = 0);
        void lock(address_t address, const String& name);
        void lock(address_t address, SymbolType type, tag_t tag = 0);
        void lock(address_t address, const String& name, SymbolType type, tag_t tag = 0);
        void segment(const String& name, offset_t offset, address_t address, u64 size, SegmentType type);
        void segment(const String& name, offset_t offset, address_t address, u64 psize, u64 vsize, SegmentType type);

    public:
        size_t segmentsCount() const;
        size_t lastLine() const;
        size_t itemIndex(const ListingItem* item) const;
        size_t functionIndex(address_t address) const;
        size_t instructionIndex(address_t address) const;
        size_t symbolIndex(address_t address) const;
        const ListingFunctions *functions() const;
        const ListingCursor* cursor() const;
        const Segment *segmentByName(const String& name) const;
        const Segment *segment(address_t address) const;
        const Symbol *documentEntry() const;
        const List& segments() const;
        const SymbolTable* symbols() const;
        ListingItem* functionStart(ListingItem *item) const;
        ListingItem* functionStart(address_t address) const;
        ListingItem* currentFunction() const;
        ListingItem* currentItem() const;
        ListingItem* segmentItem(address_t address) const;
        ListingItem* symbolItem(address_t address) const;
        ListingItem* instructionItem(address_t address) const;
        ListingItem* functionItem(address_t address) const;
        Segment* segment(address_t address);
        Symbol *functionStartSymbol(address_t address);
        Symbol *symbol(address_t address) const;
        Symbol *symbol(const String& name) const;
        ListingCursor* cursor();
        ListingItem* itemAt(size_t i) const;
        ListingItem* next(ListingItem* item) const;
        ListingItem* prev(ListingItem* item) const;
        ListingItem* nextInstructionItem(ListingItem* item);
        ListingItem* prevInstructionItem(ListingItem* item);
        ListingFunctions* functions();
        CachedInstruction cacheInstruction(address_t address);
        CachedInstruction instruction(address_t address);
        CachedInstruction nextInstruction(const CachedInstruction& instruction);
        CachedInstruction prevInstruction(const CachedInstruction& instruction);
        CachedInstruction nearestInstruction(address_t address);
        CachedInstruction entryInstruction();
        bool isInstructionCached(address_t address) const;
        void rename(address_t address, const String& name);
        void lockFunction(address_t address, const String& name, u32 tag = 0);
        void eraseSymbol(address_t address);
        void setDocumentEntry(address_t address);
        void instruction(const CachedInstruction& instruction);
};

typedef safe_ptr<ListingDocumentType> ListingDocument;
using document_s_lock = s_locked_safe_ptr<ListingDocument>;
using document_x_lock = x_locked_safe_ptr<ListingDocument>;

} // namespace REDasm
