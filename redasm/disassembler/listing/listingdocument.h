#pragma once

#include <unordered_set>
#include "../../redasm.h"
#include "../../support/event.h"
#include "../../support/safe_ptr.h"
#include "../../support/serializer.h"
#include "../types/symboltable.h"
#include "instructioncache.h"
#include "listingfunctions.h"
#include "listingcursor.h"
#include "listingitem.h"

namespace REDasm {

class LoaderPlugin;

struct ListingDocumentChanged
{
    enum { Changed = 0, Inserted, Removed };

    ListingDocumentChanged(ListingItem* item, size_t index, size_t action = ListingDocumentChanged::Changed): item(item), index(index), action(action) { }
    bool isInserted() const { return action == ListingDocumentChanged::Inserted; }
    bool isRemoved() const { return action == ListingDocumentChanged::Removed; }

    ListingItem* item;
    size_t index, action;
};

class ListingDocumentType: public sorted_container<ListingItemPtr, ListingItemPtrComparator>, public Serializer::Serializable
{
    public:
        Event<const ListingDocumentChanged*> changed;

    private:
        typedef sorted_container<ListingItemPtr, ListingItemPtrComparator> ContainerType;
        typedef std::unordered_map<address_t, Detail::CommentSet> PendingAutoComments;
        typedef std::unordered_map<address_t, size_t> ActiveMeta;

    private:
        using ContainerType::insert;
        using ContainerType::at;

    public:
        ListingDocumentType();
        virtual ~ListingDocumentType() = default;
        bool advance(InstructionPtr& instruction);
        bool goTo(const ListingItem* findItem);
        bool goTo(address_t address);
        void moveToEP();

    public:
        virtual void serializeTo(std::fstream& fs);
        virtual void deserializeFrom(std::fstream& fs);

    public: // ListingItemData functions
        std::string comment(const ListingItem *item, bool skipauto = false) const;
        void comment(ListingItem* item, const std::string& s);
        void autoComment(address_t address, const std::string& s);

    public: // ListingItem getter functions
        std::string type(const ListingItem *item) const;
        void type(address_t address, const std::string& s);
        const Detail::MetaItem &meta(const ListingItem *item) const;

    public: // ListingItem inserter functions
        void empty(address_t address);
        void meta(address_t address, const std::string& s, const std::string& name = std::string());
        void entry(address_t address, u32 tag = 0);
        void function(address_t address, const std::string& name, u32 tag = 0);
        void function(address_t address, u32 tag = 0);
        void pointer(address_t address, u32 type, u32 tag = 0);
        void table(address_t address, u64 count, u32 tag = 0);
        void tableItem(address_t address, address_t startaddress, u64 idx, u32 tag = 0);
        void branch(address_t address, s64 direction, u32 tag = 0);
        void symbol(address_t address, const std::string& name, u32 type, u32 tag = 0);
        void symbol(address_t address, u32 type, u32 tag = 0);
        void lock(address_t address, const std::string& name);
        void lock(address_t address, u32 type, u32 tag = 0);
        void lock(address_t address, const std::string& name, u32 type, u32 tag = 0);
        void segment(const std::string& name, offset_t offset, address_t address, u64 size, u64 type);
        void segment(const std::string& name, offset_t offset, address_t address, u64 psize, u64 vsize, u64 type);

    public:
        size_t segmentsCount() const;
        size_t lastLine() const;
        const ListingCursor* cursor() const;
        ListingCursor* cursor();
        const Segment *segmentByName(const std::string& name) const;
        const Segment *segment(address_t address) const;
        Segment* segment(address_t address);
        ListingFunctions& functions();
        const ListingFunctions& functions() const;
        const SegmentList& segments() const;
        const SymbolTable* symbols() const;
        Symbol *functionStartSymbol(address_t address);
        ListingItem* functionStart(ListingItem* findItem) const;
        ListingItem* functionStart(address_t address) const;
        ListingItem* currentFunction() const;
        ListingItem* currentItem() const;
        InstructionPtr entryInstruction();
        void rename(address_t address, const std::string& name);
        void lockFunction(address_t address, const std::string& name, u32 tag = 0);
        void eraseSymbol(address_t address);
        void setDocumentEntry(address_t address);
        const Symbol *documentEntry() const;
        void instruction(const InstructionPtr& instruction);
        void update(const InstructionPtr& instruction);
        InstructionPtr instruction(address_t address);
        const_iterator functionItem(address_t address) const;
        ListingDocumentType::const_iterator instructionItem(address_t address) const;
        ListingDocumentType::const_iterator symbolItem(address_t address) const;
        size_t itemIndex(const ListingItem* item) const;
        size_t functionIndex(address_t address) const;
        size_t instructionIndex(address_t address) const;
        size_t symbolIndex(address_t address) const;
        ListingItem* itemAt(size_t i) const;
        Symbol *symbol(address_t address) const;
        Symbol *symbol(const std::string& name) const;

    private:
        ListingDocumentType::const_iterator findItem(address_t address, size_t type, size_t index = 0) const;
        size_t findIndex(address_t address, size_t type, size_t index = 0) const;
        ListingItem* push(address_t address, size_t type, size_t index = 0);
        void pop(address_t address, size_t type);

    private:
        ListingCursor m_cursor;
        PendingAutoComments m_pendingautocomments;
        SegmentList m_segments;
        ListingFunctions m_functions;
        InstructionCache m_instructions;
        SymbolTable m_symboltable;
        Symbol* m_documententry;
        ActiveMeta m_activemeta;

    friend class LoaderPlugin;
};

typedef safe_ptr<ListingDocumentType> ListingDocument;
using document_s_lock = s_locked_safe_ptr<ListingDocument>;
using document_x_lock = x_locked_safe_ptr<ListingDocument>;

} // namespace REDasm
