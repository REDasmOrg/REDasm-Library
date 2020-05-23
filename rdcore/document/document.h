#pragma once

#include <unordered_set>
#include <unordered_map>
#include <map>
#include <rdapi/document/document.h>
#include <rdapi/events.h>
#include "../object.h"
#include "document_fwd.h"

class FunctionGraph;
class FunctionContainer;
class SegmentContainer;
class InstructionCache;
class BlockContainer;
class ItemContainer;
class SymbolTable;

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
        virtual ~Document();
        bool isInstructionCached(address_t address) const;
        const SymbolTable* symbols() const;
        const BlockContainer* blocks() const;
        const RDSymbol* entry() const;

    public: // Insertion
        void segment(const std::string& name, offset_t offset, address_t address, u64 psize, u64 vsize, flag_t flags);
        void imported(address_t address, size_t size, const std::string& name);
        void exported(address_t address, const std::string& name);
        void exportedFunction(address_t address, const std::string& name);
        void instruction(const RDInstruction* instruction);
        void asciiString(address_t address, size_t size);
        void wideString(address_t address, size_t size);
        void data(address_t address, size_t size, const std::string& name);
        void table(address_t address, size_t count);
        void tableItem(address_t address, address_t startaddress, u64 idx);
        void pointer(address_t address, type_t type, const std::string& name);
        void function(address_t address, const std::string& name);
        void branch(address_t address, int direction);
        void type(address_t address, const std::string& s);
        void separator(address_t address);
        void label(address_t address);
        void entry(address_t address);
        void empty(address_t address);

    public: // Count
        size_t blocksCount() const;
        size_t itemsCount() const;
        size_t segmentsCount() const;
        size_t functionsCount() const;
        size_t symbolsCount() const;
        bool empty() const;

    public: // Data
        std::string comment(address_t address, bool skipauto, const char* separator) const;
        void autoComment(address_t address, const std::string& s);
        void comment(address_t address, const std::string& s);
        void rename(address_t address, const std::string& newname);

    public: // Get-i
        size_t itemsAt(size_t startidx, size_t count, RDDocumentItem* item) const;
        bool itemAt(size_t idx, RDDocumentItem* item) const;
        bool blockAt(size_t idx, RDBlock* block) const;
        bool segmentAt(size_t idx, RDSegment* segment) const;
        RDLocation functionAt(size_t idx) const;

    public: // Get
        RDLocation entryPoint() const;
        bool prevInstruction(const RDInstruction* instruction, RDInstruction** previnstruction) const;
        bool lockInstruction(address_t address, RDInstruction** instruction) const;
        bool unlockInstruction(const RDInstruction* instruction) const;
        bool symbol(const char* name, RDSymbol* symbol) const;
        bool symbol(address_t address, RDSymbol* symbol) const;
        bool block(address_t address, RDBlock* block) const;
        bool segment(address_t address, RDSegment* segment) const;
        bool segmentOffset(offset_t offset, RDSegment* segment) const;
        const char* name(address_t address) const;

    public: // Index
        size_t itemIndex(const RDDocumentItem* item) const;
        size_t functionIndex(address_t address) const;
        size_t instructionIndex(address_t address) const;
        size_t symbolIndex(address_t address) const;

    public: // Item
        bool functionItem(address_t address, RDDocumentItem* item) const;
        bool instructionItem(address_t address, RDDocumentItem* item) const;
        bool symbolItem(address_t address, RDDocumentItem* item) const;

    public: // Graph
        void invalidateGraphs();
        void graph(FunctionGraph* g);
        FunctionGraph* graph(address_t address) const;
        RDLocation functionStart(address_t address) const;

    private:
        void block(address_t address, size_t size, const std::string& name, type_t type, flag_t flags);
        void symbol(address_t address, const std::string& name, type_t type, flag_t flags);

    private:
        const RDDocumentItem& insert(address_t address, type_t type, u16 index = 0);
        void notify(size_t idx, type_t action);
        void replace(address_t address, type_t type);
        void remove(address_t address, type_t type);
        void removeAt(size_t idx);
        bool canSymbolizeAddress(address_t address, flag_t flags) const;

    private:
        static void onBlockInserted(const RDEventArgs* e, void* userdata);
        static void onBlockRemoved(const RDEventArgs* e, void* userdata);

    private:
        RDSymbol m_entry{ };
        std::unordered_map<address_t, ItemData> m_itemdata;
        std::unordered_set<event_t> m_events;
        std::unordered_set<address_t> m_separators;
        std::unique_ptr<InstructionCache> m_instructions;
        std::unique_ptr<SegmentContainer> m_segments;
        std::unique_ptr<FunctionContainer> m_functions;
        std::unique_ptr<BlockContainer> m_blocks;
        std::unique_ptr<ItemContainer> m_items;
        std::unique_ptr<SymbolTable> m_symbols;
};
