#ifndef FUNCTIONGRAPH_H
#define FUNCTIONGRAPH_H

#include "../disassemblerapi.h"
#include "../listing/listingdocument.h"
#include "../../graph/graph.h"
#include <queue>

namespace REDasm {
namespace Graphing {

struct FunctionBasicBlock: public Node
{
    s64 startidx, endidx; // [startidx, endidx]
    std::unordered_map<const FunctionBasicBlock*, std::string> styles;

    FunctionBasicBlock(s64 startidx): startidx(startidx), endidx(startidx) { }
    bool contains(s64 index) const { return (index >= startidx) && (index <= endidx); }
    bool isEmpty() const { return startidx > endidx; }
    s64 count() const { return (endidx - startidx) + 1; }
    void bTrue(const FunctionBasicBlock* v) { styles[v] = "graph_edge_true"; }
    void bFalse(const FunctionBasicBlock* v) { styles[v] = "graph_edge_false"; }
    void bLoop(const FunctionBasicBlock* v) { styles[v] = "graph_edge_loop"; }
    void bLoopConditional(const FunctionBasicBlock* v) { styles[v] = "graph_edge_loop_c"; }

    std::string style(const FunctionBasicBlock* to) const {
        auto it = styles.find(to);

        if(it == styles.end())
            return "graph_edge";

        return it->second;
    }
};

class FunctionGraph: public Graph
{
    private:
        typedef std::queue<s64> IndexQueue;

    public:
        FunctionGraph(DisassemblerAPI* disassembler);
        address_location startAddress() const;
        bool build(address_t address);

    protected:
        virtual bool compareEdge(const Node *n1, const Node *n2) const;

    private:
        FunctionBasicBlock* basicBlockFromIndex(s64 index) const;
        void setConnectionType(const InstructionPtr& instruction, FunctionBasicBlock* fromfbb, FunctionBasicBlock* tofbb, bool condition);
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

#endif // FUNCTIONGRAPH_H
