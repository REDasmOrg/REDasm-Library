#pragma once

#include <queue>
#include "../disassembler/disassemblerapi.h"
#include "../disassembler/listing/listingdocument.h"
#include "graph.h"

namespace REDasm {
namespace Graphing {

struct FunctionBasicBlock
{
    Node node;
    s64 startidx, endidx; // [startidx, endidx]
    std::unordered_map<Node, std::string> styles;

    FunctionBasicBlock(): startidx(-1), endidx(startidx) { }
    FunctionBasicBlock(s64 startidx): startidx(startidx), endidx(startidx) { }
    bool contains(s64 index) const { return (index >= startidx) && (index <= endidx); }
    bool isEmpty() const { return startidx > endidx; }
    s64 count() const { return (endidx - startidx) + 1; }
    void bTrue(const Node& n) { styles[n] = "graph_edge_true"; }
    void bFalse(const Node& n) { styles[n] = "graph_edge_false"; }

    std::string style(const Node& n) const {
        auto it = styles.find(n);

        if(it == styles.end())
            return "graph_edge";

        return it->second;
    }
};

class FunctionGraph: public GraphT<FunctionBasicBlock>
{
    private:
        typedef std::queue<size_t> IndexQueue;

    public:
        FunctionGraph(DisassemblerAPI* disassembler);
        bool build(address_t address);

    private:
        FunctionBasicBlock* basicBlockFromIndex(s64 index);
        void setConnectionType(const InstructionPtr& instruction, FunctionBasicBlock *fromfbb, FunctionBasicBlock *tofbb, bool condition);
        void incomplete() const;
        bool isStopItem(ListingItem* item);
        void buildBasicBlock(s64 index, IndexQueue &pending);
        void buildBasicBlocks();
        bool connectBasicBlocks();

    private:
        s64 instructionIndexFromIndex(s64 idx) const;
        s64 symbolIndexFromIndex(s64 idx) const;

    private:
        DisassemblerAPI* m_disassembler;
        ListingDocument& m_document;
        address_location m_graphstart;
};

} // namespace Graphing
} // namespace REDasm
