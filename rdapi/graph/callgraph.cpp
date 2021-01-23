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

bool RDCallGraph_WalkFrom(RDGraph* graph, rd_address address)
{
    auto* cg = dynamic_cast<CallGraph*>(CPTR(Graph, graph));
    if(!cg) return false;
    cg->walkFrom(address);
    return true;
}

RDGraphNode RDCallGraphItem_GetNode(const RDCallGraphItem* item) { return CPTR(const CallGraphItem, item)->node(); }
const RDNetNode* RDCallGraphItem_GetNetNode(const RDCallGraphItem* item) { return CPTR(const RDNetNode, CPTR(const CallGraphItem, item)->netNode()); }
