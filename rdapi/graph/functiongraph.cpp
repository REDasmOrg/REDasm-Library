#include "functiongraph.h"
#include <rdcore/builtin/graph/functiongraph.h>

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

const char* RDFunctionBasicBlock_GetStyle(const RDFunctionBasicBlock* fbb, RDGraphNode node) { return CPTR(const FunctionBasicBlock, fbb)->getStyle(node); }
size_t RDFunctionBasicBlock_ItemsCount(const RDFunctionBasicBlock* fbb) { return CPTR(const FunctionBasicBlock, fbb)->itemsCount(); }
size_t RDFunctionBasicBlock_GetStartIndex(const RDFunctionBasicBlock* fbb) { return CPTR(const FunctionBasicBlock, fbb)->startIndex(); }
size_t RDFunctionBasicBlock_GetEndIndex(const RDFunctionBasicBlock* fbb) { return CPTR(const FunctionBasicBlock, fbb)->endIndex(); }
address_t RDFunctionBasicBlock_GetStartAddress(const RDFunctionBasicBlock* fbb) { return CPTR(const FunctionBasicBlock, fbb)->startaddress; }
address_t RDFunctionBasicBlock_GetEndAddress(const RDFunctionBasicBlock* fbb) { return CPTR(const FunctionBasicBlock, fbb)->endaddress; }
bool RDFunctionBasicBlock_GetStartItem(const RDFunctionBasicBlock* fbb, RDDocumentItem* item) { return CPTR(const FunctionBasicBlock, fbb)->getStartItem(item); }
bool RDFunctionBasicBlock_GetEndItem(const RDFunctionBasicBlock* fbb, RDDocumentItem* item) { return CPTR(const FunctionBasicBlock, fbb)->getEndItem(item); }
bool RDFunctionBasicBlock_Contains(const RDFunctionBasicBlock* fbb, address_t address) { return CPTR(const FunctionBasicBlock, fbb)->contains(address); }
