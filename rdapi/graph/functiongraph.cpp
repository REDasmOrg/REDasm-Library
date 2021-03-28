#include "functiongraph.h"
#include <rdcore/builtin/graph/functiongraph/functiongraph.h>

bool RDFunctionGraph_GetBasicBlock(const RDGraph* graph, RDGraphNode n, const RDFunctionBasicBlock** fbb)
{
    const Graph* g = CPTR(const Graph, graph);
    const FunctionGraph* fg = dynamic_cast<const FunctionGraph*>(g);
    if(!fg) return false;

    const RDGraphData* data = fg->data(n);
    if(!data->p_data) return false;

    if(fbb) *fbb = CPTR(RDFunctionBasicBlock, data->p_data);
    return true;
}

bool RDFunctionGraph_Contains(const RDGraph* graph, rd_address address)
{
    const Graph* g = CPTR(const Graph, graph);
    const FunctionGraph* fg = dynamic_cast<const FunctionGraph*>(g);
    return fg ? (fg->basicBlock(address) != nullptr) : false;
}

rd_address RDFunctionGraph_GetStartAddress(const RDGraph* graph)
{
    const Graph* g = CPTR(const Graph, graph);
    const FunctionGraph* fg = dynamic_cast<const FunctionGraph*>(g);
    return fg ? fg->startAddress() : RD_NVAL;
}

rd_address RDFunctionGraph_GetEndAddress(const RDGraph* graph)
{
    const Graph* g = CPTR(const Graph, graph);
    const FunctionGraph* fg = dynamic_cast<const FunctionGraph*>(g);
    return fg ? fg->endAddress() : RD_NVAL;
}

size_t RDFunctionGraph_GetBlocksCount(const RDGraph* graph)
{
    const Graph* g = CPTR(const Graph, graph);
    const FunctionGraph* fg = dynamic_cast<const FunctionGraph*>(g);
    return fg ? fg->blocksCount() : 0;
}

rd_type RDFunctionBasicBlock_GetTheme(const RDFunctionBasicBlock* fbb, RDGraphNode node) { return CPTR(const FunctionBasicBlock, fbb)->getTheme(node); }
size_t RDFunctionBasicBlock_ItemsCount(const RDFunctionBasicBlock* fbb) { return CPTR(const FunctionBasicBlock, fbb)->itemsCount(); }
rd_address RDFunctionBasicBlock_GetStartAddress(const RDFunctionBasicBlock* fbb) { return CPTR(const FunctionBasicBlock, fbb)->startaddress; }
rd_address RDFunctionBasicBlock_GetEndAddress(const RDFunctionBasicBlock* fbb) { return CPTR(const FunctionBasicBlock, fbb)->endaddress; }
bool RDFunctionBasicBlock_Contains(const RDFunctionBasicBlock* fbb, rd_address address) { return CPTR(const FunctionBasicBlock, fbb)->contains(address); }
