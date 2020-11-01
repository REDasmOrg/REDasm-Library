#pragma once

#include <unordered_set>
#include <unordered_map>
#include <map>
#include <rdapi/document/document.h>
#include <rdapi/events.h>
#include "../document/backend/segmentcontainer.h"
#include "../document/backend/functioncontainer.h"
#include "../document/backend/itemcontainer.h"
#include "../document/backend/symboltable.h"
#include "../object.h"
#include "document_fwd.h"

class FunctionGraph;

class Document: public Object
{
    private:
        struct MetaItem { std::string name, type; };

        struct ItemData {
            std::unordered_set<std::string> comments, autocomments;
            std::map<size_t, MetaItem> meta;
            std::string type;
        };

    public:
        Document(Context* ctx);
        virtual ~Document() = default;
        size_t size() const;
        bool empty() const;
        bool contains(const RDDocumentItem* item) const;
        bool getAny(rd_address address, const rd_type* types, RDDocumentItem* item) const;
        const ItemContainer* items() const;
        const FunctionContainer* functions() const;
        const SegmentContainer* segments() const;
        const BlockContainer* blocks(rd_address address) const;
        const SymbolTable* symbols() const;
        const RDSymbol* entry() const;

    public: // Insertion
        bool segment(const std::string& name, rd_offset offset, rd_address address, u64 psize, u64 vsize, rd_flag flags);
        bool imported(rd_address address, size_t size, const std::string& name);
        bool exported(rd_address address, size_t size, const std::string& name);
        bool exportedFunction(rd_address address, const std::string& name);
        bool instruction(rd_address address, size_t size);
        bool asciiString(rd_address address, size_t size, const std::string& name);
        bool wideString(rd_address address, size_t size, const std::string& name);
        bool data(rd_address address, size_t size, const std::string& name);
        void table(rd_address address, size_t count);
        void tableItem(rd_address address, rd_address startaddress, size_t idx);
        bool pointer(rd_address address, rd_type type, const std::string& name);
        bool function(rd_address address, const std::string& name);
        bool branch(rd_address address, int direction);
        void type(rd_address address, const std::string& s);
        void separator(rd_address address);
        bool label(rd_address address);
        bool entry(rd_address address);
        void empty(rd_address address);

    public: // Data
        std::string comment(rd_address address, bool skipauto, const char* separator) const;
        void autoComment(rd_address address, const std::string& s);
        void comment(rd_address address, const std::string& s);
        bool rename(rd_address address, const std::string& newname);

    public: // Get
        RDLocation entryPoint() const;
        bool symbol(const char* name, RDSymbol* symbol) const;
        bool symbol(rd_address address, RDSymbol* symbol) const;
        bool block(rd_address address, RDBlock* block) const;
        bool segment(rd_address address, RDSegment* segment) const;
        bool segmentOffset(rd_offset offset, RDSegment* segment) const;
        const char* name(rd_address address) const;

    public: // Graph
        void invalidateGraphs();
        void graph(FunctionGraph* g);
        FunctionGraph* graph(rd_address address) const;
        RDLocation functionStart(rd_address address) const;

    public: // Segment
        bool setSegmentUserData(rd_address address, uintptr_t userdata);

    private:
        bool block(rd_address address, size_t size, const std::string& name, rd_type type, rd_flag flags);
        bool symbol(rd_address address, const std::string& name, rd_type type, rd_flag flags);

    private:
        const RDDocumentItem& insert(rd_address address, rd_type type, u16 index = 0);
        void notifyEvent(const RDDocumentItem& item, rd_type action);
        void replace(rd_address address, rd_type type);
        void remove(rd_address address, rd_type type);
        bool canSymbolizeAddress(rd_address address, rd_flag flags) const;
        void onBlockInserted(const RDBlock& b);
        void onBlockRemoved(const RDBlock& b);

    private:
        RDSymbol m_entry{ };
        ItemContainer m_items;
        SegmentContainer m_segments;
        FunctionContainer m_functions;
        std::unordered_map<rd_address, ItemData> m_itemdata;
        std::unordered_set<rd_address> m_separators;
        std::unique_ptr<SymbolTable> m_symbols;
};
