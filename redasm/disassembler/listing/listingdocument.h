#ifndef LISTINGDOCUMENT_H
#define LISTINGDOCUMENT_H

#include <unordered_set>
#include <algorithm>
#include <vector>
#include <list>
#include "../../redasm.h"
#include "../../support/event.h"
#include "../../support/safe_ptr.h"
#include "../../support/serializer.h"
#include "../types/symboltable.h"
#include "../types/referencetable.h"
#include "listingcursor.h"
#include "instructioncache.h"

namespace REDasm {

class LoaderPlugin;

struct ListingItem
{
    enum: size_t {
        Undefined = 0,
        SegmentItem, EmptyItem, FunctionItem, TypeItem, SymbolItem, MetaItem, InstructionItem,
        AllItems = static_cast<size_t>(-1)
    };

    ListingItem(): address(0), type(ListingItem::Undefined), index(0) { }
    ListingItem(address_t address, size_t type, size_t index): address(address), type(type), index(index) { }
    bool is(size_t t) const { return type == t; }

    address_t address;
    size_t type, index;
};

typedef std::unique_ptr<ListingItem> ListingItemPtr;
typedef std::deque<ListingItem*> ListingItems;

namespace Listing {
    template<typename T> struct ListingComparator {
        bool operator()(const T& t1, const T& t2) {
            if(t1->address == t2->address) {
                if(t1->type == t2->type)
                    return t1->index < t2->index;

                return t1->type < t2->type;
            }

            return t1->address < t2->address;
        }
    };

    template<typename T> struct IteratorSelector {
        typedef typename std::conditional<std::is_const<T>::value, typename T::const_iterator, typename T::iterator>::type Type;
    };

    template<typename T, typename V, typename IT = typename IteratorSelector<T>::Type> typename T::iterator insertionPoint(T* container, const V& item) {
        return std::lower_bound(container->begin(), container->end(), item, ListingComparator<V>());
    }

    template<typename T, typename IT = typename IteratorSelector<T>::Type> IT _adjustSearch(T* container, IT it, u32 type) {
        int offset = type - (*it)->type;
        address_t searchaddress = (*it)->address;

        while((it != container->end()) && (searchaddress == (*it)->address))
        {
            if((*it)->type == type)
                return it;

            if((offset < 0) && (it == container->begin()))
                break;

            offset > 0 ? it++ : it--;
        }

        return container->end();
    }

    template<typename T, typename IT = typename IteratorSelector<T>::Type> IT binarySearch(T* container, address_t address, u32 type) {
        if(!container->size())
            return container->end();

        auto thebegin = container->begin(), theend = container->end();

        if((address < (*thebegin)->address) || (address > (*container->rbegin())->address))
            return container->end();

        while(thebegin <= theend)
        {
            auto range = std::distance(thebegin, theend);
            auto themiddle = thebegin;
            std::advance(themiddle, range / 2);

            if((*themiddle)->address == address)
                return Listing::_adjustSearch(container, themiddle, type);

            if((*themiddle)->address > address)
            {
                theend = themiddle;
                std::advance(theend, -1);
            }
            else if((*themiddle)->address < address)
            {
                thebegin = themiddle;
                std::advance(thebegin, 1);
            }
        }

        return container->end();
    }

    template<typename T, typename V, typename IT = typename IteratorSelector<T>::Type> IT binarySearch(T* container, V* item) {
        return Listing::binarySearch(container, item->address, item->type);
    }

    template<typename T, typename V, typename IT = typename IteratorSelector<T>::Type> s64 indexOf(T* container, V* item) {
        auto it = Listing::binarySearch(container, item);

        if(it == container->end())
            return -1;

        return std::distance(container->begin(), it);
    }

    template<typename T, typename IT = typename IteratorSelector<T>::Type> s64 indexOf(T* container, address_t address, u32 type) {
        auto it = Listing::binarySearch(container, address, type);

        if(it == container->end())
            return -1;

        return std::distance(container->begin(), it);
    }
}

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

class ListingDocumentType: protected std::deque<ListingItemPtr>, public Serializer::Serializable
{
    public:
        Event<const ListingDocumentChanged*> changed;

    private:
        typedef std::set<std::string> CommentSet;
        typedef std::deque< std::pair<std::string, std::string> > MetaList;
        typedef std::pair<address_t, CommentSet> AutoCommentItem;
        typedef std::pair<address_t, std::string> CommentItem;
        typedef std::unordered_map<address_t, CommentSet> AutoCommentMap;
        typedef std::unordered_map<address_t, std::string> AddressStringMap;
        typedef std::unordered_map<address_t, MetaList> MetaMap;
        typedef std::deque<ListingItem*> FunctionList;

    public:
        using std::deque<ListingItemPtr>::const_iterator;
        using std::deque<ListingItemPtr>::iterator;
        using std::deque<ListingItemPtr>::begin;
        using std::deque<ListingItemPtr>::end;
        using std::deque<ListingItemPtr>::rbegin;
        using std::deque<ListingItemPtr>::rend;
        using std::deque<ListingItemPtr>::empty;
        using std::deque<ListingItemPtr>::size;

    public:
        ListingDocumentType();
        virtual ~ListingDocumentType() = default;
        bool advance(InstructionPtr& instruction);
        const ListingCursor* cursor() const;
        ListingCursor* cursor();
        void moveToEP();
        u64 length() const;
        u64 lastLine() const;

    public:
        virtual void serializeTo(std::fstream& fs);
        virtual void deserializeFrom(std::fstream& fs);

    public:
        ListingItems getCalls(ListingItem* item);
        ListingItem* functionStart(ListingItem* item);
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
        ListingDocumentType::iterator functionItem(address_t address);
        ListingDocumentType::iterator instructionItem(address_t address);
        ListingDocumentType::iterator symbolItem(address_t address);
        ListingDocumentType::iterator item(address_t address);
        s64 functionIndex(address_t address);
        s64 instructionIndex(address_t address);
        s64 symbolIndex(address_t address);
        ListingItem* itemAt(size_t i) const;
        s64 indexOf(address_t address);
        s64 indexOf(ListingItem *item);
        Symbol *symbol(address_t address) const;
        Symbol *symbol(const std::string& name) const;
        const SymbolTable* symbols() const;

    private:
        void insertSorted(address_t address, u32 type, size_t index = 0);
        void removeSorted(address_t address, u32 type);
        ListingDocumentType::iterator item(address_t address, u32 type);
        s64 index(address_t address, u32 type);
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

#endif // LISTINGDOCUMENT_H
