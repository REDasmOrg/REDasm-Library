#pragma once

#include "../../redasm.h"
#include "../../support/containers/cache_map.h"
#include "../../support/serializer.h"
#include "../../support/safe_ptr.h"
#include "../../support/event.h"
#include "../types/symboltable.h"
#include "listingfunctions.h"
#include "listingcursor.h"
#include "listingitem.h"

namespace REDasm {

class LoaderPlugin;

struct ListingDocumentChanged
{
    enum { Changed = 0, Inserted, Removed };

    ListingDocumentChanged(const ListingItem* item, size_t index, size_t action = ListingDocumentChanged::Changed): item(item), index(index), action(action) { }
    bool isInserted() const { return action == ListingDocumentChanged::Inserted; }
    bool isRemoved() const { return action == ListingDocumentChanged::Removed; }

    const ListingItem* item;
    size_t index, action;
};

class ListingDocumentType: public sorted_container<ListingItemPtr, ListingItemPtrComparator>
{
    public:
        Event<const ListingDocumentChanged*> changed;

    public:
        typedef sorted_container<ListingItemPtr, ListingItemPtrComparator> ContainerType;

    private:
        typedef cache_map<address_t, InstructionPtr> InstructionCache;
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

    public: // ListingItemData functions
        std::string comment(const ListingItem *item, bool skipauto = false) const;
        void comment(const ListingItem* item, const std::string& s);
        void autoComment(address_t address, const std::string& s);

    public: // ListingItem getter functions
        std::string type(const ListingItem *item) const;
        void type(address_t address, const std::string& s);
        const Detail::MetaItem &meta(const ListingItem *item) const;

    public: // ListingItem inserter functions
        void empty(address_t address);
        void meta(address_t address, const std::string& s, const std::string& name = std::string());
        void entry(address_t address, tag_t tag = 0);
        void function(address_t address, const std::string& name, tag_t tag = 0);
        void function(address_t address, tag_t tag = 0);
        void pointer(address_t address, SymbolType type, tag_t tag = 0);
        void table(address_t address, u64 count, tag_t tag = 0);
        void tableItem(address_t address, address_t startaddress, u64 idx, tag_t tag = 0);
        void branch(address_t address, s64 direction, tag_t tag = 0);
        void symbol(address_t address, const std::string& name, SymbolType type, tag_t tag = 0);
        void symbol(address_t address, SymbolType type, tag_t tag = 0);
        void lock(address_t address, const std::string& name);
        void lock(address_t address, SymbolType type, tag_t tag = 0);
        void lock(address_t address, const std::string& name, SymbolType type, tag_t tag = 0);
        void segment(const std::string& name, offset_t offset, address_t address, u64 size, SegmentType type);
        void segment(const std::string& name, offset_t offset, address_t address, u64 psize, u64 vsize, SegmentType type);

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
        const ListingItem* functionStart(const ListingItem* item) const;
        const ListingItem* functionStart(address_t address) const;
        const ListingItem* currentFunction() const;
        const ListingItem* currentItem() const;
        InstructionPtr entryInstruction();
        void rename(address_t address, const std::string& name);
        void lockFunction(address_t address, const std::string& name, u32 tag = 0);
        void eraseSymbol(address_t address);
        void setDocumentEntry(address_t address);
        const Symbol *documentEntry() const;
        void instruction(const InstructionPtr& instruction);
        void update(const InstructionPtr& instruction);
        InstructionPtr instruction(address_t address);
        const_iterator functionStartItem(address_t address) const;
        const_iterator functionItem(address_t address) const;
        const_iterator instructionItem(address_t address) const;
        const_iterator symbolItem(address_t address) const;
        const_iterator segmentItem(address_t address) const;
        size_t itemIndex(const ListingItem* item) const;
        size_t functionIndex(address_t address) const;
        size_t instructionIndex(address_t address) const;
        size_t symbolIndex(address_t address) const;
        ListingItem* itemAt(size_t i) const;
        Symbol *symbol(address_t address) const;
        Symbol *symbol(const std::string& name) const;

    private:
        ListingDocumentType::const_iterator findItem(address_t address, size_t type, size_t index = 0) const;
        ListingDocumentType::const_iterator findItem(const ListingItem* item) const;
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
    friend struct Serializer< safe_ptr<ListingDocumentType> >;
};

typedef safe_ptr<ListingDocumentType> ListingDocument;
using document_s_lock = s_locked_safe_ptr<ListingDocument>;
using document_x_lock = x_locked_safe_ptr<ListingDocument>;

template<> struct Serializer<ListingDocument> {
    static void write(std::fstream& fs, const ListingDocument& d) {
        auto lock = x_lock_safe_ptr(d);

        Serializer<SegmentList>::write(fs, lock->m_segments);
        Serializer<SymbolTable>::write(fs, &lock->m_symboltable);

        Serializer<typename ListingDocumentType::ContainerType>::write(fs, *lock.t.get());

        Serializer<address_t>::write(fs, (lock->m_documententry ? lock->m_documententry->address : 0));
        Serializer<ListingCursor>::write(fs, &lock->m_cursor);
    }

    static void read(std::fstream& fs, ListingDocument& d, const std::function<InstructionPtr(address_t address)> cb) {
        auto lock = x_lock_safe_ptr(d);

        Serializer<SegmentList>::read(fs, lock->m_segments);
        Serializer<SymbolTable>::read(fs, &lock->m_symboltable);

        Serializer<typename ListingDocumentType::ContainerType>::read(fs, [&](ListingItemPtr item) {
            if(item->is(ListingItem::InstructionItem))
                lock->m_instructions.commit(item->address, cb(item->address));

            lock->insert(std::move(item));
        });

        address_t entry = 0;
        Serializer<address_t>::read(fs, entry);
        lock->m_documententry = lock->symbol(entry);

        Serializer<ListingCursor>::read(fs, &lock->m_cursor);
    }
};


} // namespace REDasm
