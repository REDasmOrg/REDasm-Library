#pragma once

#include <unordered_set>
#include "../../redasm.h"
#include "../../support/event.h"
#include "../../support/safe_ptr.h"
#include "../../support/serializer.h"
#include "../types/symboltable.h"
#include "../types/referencetable.h"
#include "instructioncache.h"
#include "listingcursor.h"
#include "listingitem.h"

namespace REDasm {

class LoaderPlugin;

struct ListingDocumentChanged
{
    enum { Changed = 0, Inserted, Removed };

    ListingDocumentChanged(ListingItem* item, u64 index, size_t action = ListingDocumentChanged::Changed): item(item), index(index), action(action) { }
    bool isInserted() const { return action == ListingDocumentChanged::Inserted; }
    bool isRemoved() const { return action == ListingDocumentChanged::Removed; }

    ListingItem* item;

    u64 index;
    size_t action;
};

class ListingDocumentType: public sorted_container<ListingItemPtr, ListingItemPtrComparator>, public Serializer::Serializable
{
    public:
        Event<const ListingDocumentChanged*> changed;

    private:
        typedef sorted_container<ListingItemPtr, ListingItemPtrComparator> ContainerType;

    private:
        typedef std::set<std::string> CommentSet;
        typedef std::deque< std::pair<std::string, std::string> > MetaList;
        typedef std::pair<address_t, CommentSet> AutoCommentItem;
        typedef std::pair<address_t, std::string> CommentItem;
        typedef std::unordered_map<address_t, CommentSet> AutoCommentMap;
        typedef std::unordered_map<address_t, std::string> AddressStringMap;
        typedef std::unordered_map<address_t, MetaList> MetaMap;
        typedef ListingItemContainer FunctionList;

    private:
        using ContainerType::insert;

    public:
        ListingDocumentType();
        virtual ~ListingDocumentType() = default;
        bool advance(InstructionPtr& instruction);
        const ListingCursor* cursor() const;
        ListingCursor* cursor();
        bool goTo(const ListingItem* findItem);
        bool goTo(address_t address);
        void moveToEP();
        u64 length() const;
        u64 lastLine() const;

    public:
        virtual void serializeTo(std::fstream& fs);
        virtual void deserializeFrom(std::fstream& fs);

    public:
        ListingItem* functionStart(ListingItem* findItem);
        ListingItem* functionStart(address_t address);
        ListingItem* currentFunction();
        ListingItem* currentItem();
        Symbol *functionStartSymbol(address_t address);
        InstructionPtr entryInstruction();
        std::string comment(address_t address, bool skipauto = false) const;
        std::pair<std::string, std::string> meta(address_t address, size_t index) const;
        std::string type(address_t address) const;
        void empty(address_t address);
        void meta(address_t address, const std::string& s, const std::string& name = std::string());
        void type(address_t address, const std::string& s);
        void comment(address_t address, const std::string& s);
        void autoComment(address_t address, const std::string& s);
        void branch(address_t address, s64 direction, u32 tag = 0);
        void symbol(address_t address, const std::string& name, u32 type, u32 tag = 0);
        void symbol(address_t address, u32 type, u32 tag = 0);
        void rename(address_t address, const std::string& name);
        void lock(address_t address, const std::string& name);
        void lock(address_t address, u32 type, u32 tag = 0);
        void lock(address_t address, const std::string& name, u32 type, u32 tag = 0);
        void segment(const std::string& name, offset_t offset, address_t address, u64 size, u64 type);
        void segment(const std::string& name, offset_t offset, address_t address, u64 psize, u64 vsize, u64 type);
        void lockFunction(address_t address, const std::string& name, u32 tag = 0);
        void function(address_t address, const std::string& name, u32 tag = 0);
        void function(address_t address, u32 tag = 0);
        void pointer(address_t address, u32 type, u32 tag = 0);
        void table(address_t address, u64 count, u32 tag = 0);
        void tableItem(address_t address, address_t startaddress, u64 idx, u32 tag = 0);
        void entry(address_t address, u32 tag = 0);
        void eraseSymbol(address_t address);
        void setDocumentEntry(address_t address);
        const Symbol *documentEntry() const;
        size_t segmentsCount() const;
        size_t functionsCount() const;
        Segment *segment(address_t address);
        const Segment *segment(address_t address) const;
        const Segment *segmentAt(size_t idx) const;
        const Segment *segmentByName(const std::string& name) const;
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
        s64 indexOfz(address_t address);
        Symbol *symbol(address_t address) const;
        Symbol *symbol(const std::string& name) const;
        const SymbolTable* symbols() const;

    private:
        void insertSorted(address_t address, u32 type, size_t index = 0);
        void removeSorted(address_t address, u32 type);
        ListingDocumentType::const_iterator findItem(address_t address, size_t type, size_t index = 0) const;
        size_t findIndex(address_t address, size_t type, size_t index = 0) const;
        std::string autoComment(address_t address) const;
        static std::string normalized(std::string s);
        static std::string symbolName(const std::string& prefix, address_t address, const Segment* segment = nullptr);

    private:
        ListingCursor m_cursor;
        SegmentList m_segments;
        FunctionList m_functions;
        InstructionCache m_instructions;
        SymbolTable m_symboltable;
        Symbol* m_documententry;
        AutoCommentMap m_autocomments;
        AddressStringMap m_comments, m_types;
        MetaMap m_meta;

        friend class LoaderPlugin;
};

typedef safe_ptr<ListingDocumentType> ListingDocument;
using document_s_lock = s_locked_safe_ptr<ListingDocument>;
using document_x_lock = x_locked_safe_ptr<ListingDocument>;

} // namespace REDasm
