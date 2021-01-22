#include "callgraph.h"
#include <rdcore/builtin/graph/callgraph/callgraph.h>

RDGraph* RDCallGraph_Create(RDContext* ctx) { return CPTR(RDGraph, new CallGraph(CPTR(Context, ctx))); }

bool RDCallGraph_Walk(RDGraph* graph, rd_address address)
{
    auto* cg = dynamic_cast<CallGraph*>(CPTR(Graph, graph));
    if(!cg) return false;
    cg->walk(address);
    return true;
}

RDGraphNode RDCallGraphItem_GetNode(const RDCallGraphItem* item) { return CPTR(const CallGraphItem, item)->node(); }
rd_address RDCallGraphItem_GetAddress(const RDCallGraphItem* item) { return CPTR(const CallGraphItem, item)->address(); }
size_t RDCallGraphItem_GetSize(const RDCallGraphItem* item) { return CPTR(const CallGraphItem, item)->size(); }
const RDILExpression* RDCallGraphItem_GetCall(const RDCallGraphItem* item, size_t idx) { return CPTR(const RDILExpression, CPTR(const CallGraphItem, item)->call(idx)); }
