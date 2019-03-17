#ifndef FUNCTIONGRAPH_H
#define FUNCTIONGRAPH_H

#include "../listing/listingdocument.h"
#include "../../graph/graph.h"
#include <queue>

namespace REDasm {
namespace Graphing {

struct FunctionBlock: public Node
{
    s64 startidx, endidx; // [startidx, endidx]
    bool labelbreak;

    std::unordered_map<const FunctionBlock*, std::string> styles;

    FunctionBlock(s64 startidx): startidx(startidx), endidx(startidx), labelbreak(false) { }
    bool contains(s64 index) const { return (index >= startidx) && (index <= endidx); }
    s64 count() const { return (endidx - startidx) + 1; }
    void bTrue(const FunctionBlock* v) { styles[v] = "graph_edge_true"; }
    void bFalse(const FunctionBlock* v) { styles[v] = "graph_edge_false"; }
    void bLoop(const FunctionBlock* v) { styles[v] = "graph_edge_loop"; }
    void bLoopConditional(const FunctionBlock* v) { styles[v] = "graph_edge_loop_c"; }

    std::string style(const FunctionBlock* to) const {
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
        FunctionGraph(ListingDocument& document);
        address_location startAddress() const;
        bool build(address_t address);

    protected:
        virtual bool compareEdge(const Node *n1, const Node *n2) const;

    private:
        FunctionBlock* vertexFromListingIndex(s64 index) const;
        void buildVertices(address_t startaddress);
        void buildNode(s64 index, IndexQueue &indexqueue);
        bool isValidFirstItem(ListingItem* item);
        bool isValidItem(ListingItem* item);
        bool buildEdges();

    private:
        ListingDocument& m_document;
        address_location m_graphstart;
};

} // namespace Graphing
} // namespace REDasm

#endif // FUNCTIONGRAPH_H
