#pragma once

#include <unordered_set>
#include <unordered_map>
#include <map>
#include <rdapi/document/document.h>
#include <rdapi/events.h>
#include "../document/backend/segmentcontainer.h"
#include "../document/backend/functioncontainer.h"
#include "../document/backend/instructioncache.h"
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
        Document();
        virtual ~Document() = default;
        bool isInstructionCached(rd_address address) const;
        const BlockContainer* blocks(rd_address address) const;
        const SymbolTable* symbols() const;
        const RDSymbol* entry() const;

    public: // Insertion
        void segment(const std::string& name, rd_offset offset, rd_address address, u64 psize, u64 vsize, rd_flag flags);
        void imported(rd_address address, size_t size, const std::string& name);
        void exported(rd_address address, size_t size, const std::string& name);
        void exportedFunction(rd_address address, const std::string& name);
        void instruction(const RDInstruction* instruction);
        void asciiString(rd_address address, size_t size, const std::string& name);
        void wideString(rd_address address, size_t size, const std::string& name);
        void data(rd_address address, size_t size, const std::string& name);
        void table(rd_address address, size_t count);
        void tableItem(rd_address address, rd_address startaddress, size_t idx);
        void pointer(rd_address address, rd_type type, const std::string& name);
        void function(rd_address address, const std::string& name);
        void branch(rd_address address, int direction);
        void type(rd_address address, const std::string& s);
        void separator(rd_address address);
        void label(rd_address address);
        void entry(rd_address address);
        void empty(rd_address address);

    public: // Count
        size_t itemsCount() const;
        size_t segmentsCount() const;
        size_t functionsCount() const;
        size_t symbolsCount() const;
        bool empty() const;

    public: // Data
        std::string comment(rd_address address, bool skipauto, const char* separator) const;
        void autoComment(rd_address address, const std::string& s);
        void comment(rd_address address, const std::string& s);
        bool rename(rd_address address, const std::string& newname);

    public: // Get-i
        size_t itemsAt(size_t startidx, size_t count, RDDocumentItem* item) const;
        bool itemAt(size_t idx, RDDocumentItem* item) const;
        bool segmentAt(size_t idx, RDSegment* segment) const;
        RDLocation functionAt(size_t idx) const;

    public: // Get
        RDLocation entryPoint() const;
        bool prevInstruction(const RDInstruction* instruction, RDInstruction** previnstruction) const;
        bool lockInstruction(rd_address address, RDInstruction** instruction) const;
        bool unlockInstruction(const RDInstruction* instruction) const;
        bool symbol(const char* name, RDSymbol* symbol) const;
        bool symbol(rd_address address, RDSymbol* symbol) const;
        bool block(rd_address address, RDBlock* block) const;
        bool segment(rd_address address, RDSegment* segment) const;
        bool segmentOffset(rd_offset offset, RDSegment* segment) const;
        const char* name(rd_address address) const;

    public: // Index
        size_t itemIndex(const RDDocumentItem* item) const;
        size_t functionIndex(rd_address address) const;
        size_t instructionIndex(rd_address address) const;
        size_t symbolIndex(rd_address address) const;

    public: // Item
        bool functionItem(rd_address address, RDDocumentItem* item) const;
        bool instructionItem(rd_address address, RDDocumentItem* item) const;
        bool symbolItem(rd_address address, RDDocumentItem* item) const;

    public: // Graph
        void invalidateGraphs();
        void graph(FunctionGraph* g);
        FunctionGraph* graph(rd_address address) const;
        RDLocation functionStart(rd_address address) const;

    public: // UserData
        bool setSegmentUserData(rd_address address, uintptr_t userdata);

    private:
        void block(rd_address address, size_t size, const std::string& name, rd_type type, rd_flag flags);
        void symbol(rd_address address, const std::string& name, rd_type type, rd_flag flags);

    private:
        const RDDocumentItem& insert(rd_address address, rd_type type, u16 index = 0);
        void notify(size_t idx, rd_type action);
        void replace(rd_address address, rd_type type);
        void remove(rd_address address, rd_type type);
        void removeAt(size_t idx);
        bool canSymbolizeAddress(rd_address address, rd_flag flags) const;
        void onBlockInserted(const RDBlock& b);
        void onBlockRemoved(const RDBlock& b);

    private:
        RDSymbol m_entry{ };
        std::unordered_map<rd_address, ItemData> m_itemdata;
        std::unordered_set<rd_address> m_separators;
        std::unique_ptr<InstructionCache> m_instructions;
        std::unique_ptr<SegmentContainer> m_segments;
        std::unique_ptr<FunctionContainer> m_functions;
        std::unique_ptr<ItemContainer> m_items;
        std::unique_ptr<SymbolTable> m_symbols;
};
