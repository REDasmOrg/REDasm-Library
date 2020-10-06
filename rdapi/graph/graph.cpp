#include "graph.h"
#include <rdcore/graph/styledgraph.h>

RDGraph* RDGraph_Create(RDContext* ctx) { return CPTR(RDGraph, new StyledGraph(CPTR(Context, ctx))); }
const RDGraphEdge* RDGraph_GetEdge(const RDGraph* graph, RDGraphNode source, RDGraphNode target) { return CPTR(const Graph, graph)->edge(source, target); }
RDGraphNode RDGraph_PushNode(RDGraph* graph) { return CPTR(Graph, graph)->pushNode(); }
RDGraphNode RDGraph_GetRoot(const RDGraph* graph) { return CPTR(const Graph, graph)->root(); }
size_t RDGraph_GetOutgoing(const RDGraph* graph, RDGraphNode n, const RDGraphEdge** edges) { return CPTR(const Graph, graph)->outgoing(n, edges); }
size_t RDGraph_GetIncoming(const RDGraph* graph, RDGraphNode n, const RDGraphEdge** edges) { return CPTR(const Graph, graph)->incoming(n, edges); }
size_t RDGraph_GetNodes(const RDGraph* graph, const RDGraphNode** nodes) { return CPTR(const Graph, graph)->nodes(nodes); }
size_t RDGraph_GetEdges(const RDGraph* graph, const RDGraphEdge** edges) { return CPTR(const Graph, graph)->edges(edges); }
bool RDGraph_ContainsEdge(const RDGraph* graph, RDGraphNode source, RDGraphNode target) { return CPTR(const Graph, graph)->containsEdge(source, target); }
bool RDGraph_IsEmpty(const RDGraph* graph) { return CPTR(const Graph, graph)->empty(); }
void RDGraph_PushEdge(RDGraph* graph, RDGraphNode source, RDGraphNode target) { return CPTR(Graph, graph)->pushEdge(source, target); }
void RDGraph_SetRoot(RDGraph* graph, RDGraphNode n) { CPTR(Graph, graph)->setRoot(n); }
void RDGraph_RemoveEdge(RDGraph* graph, const RDGraphEdge* e) { CPTR(Graph, graph)->removeEdge(e); }
void RDGraph_RemoveNode(RDGraph* graph, RDGraphNode n) { CPTR(Graph, graph)->removeNode(n); }
u32 RDGraph_Hash(const RDGraph* graph) { return CPTR(const Graph, graph)->hash(); }

const char* RDGraph_GenerateDOT(const RDGraph* graph)
{
    static std::string s;
    s = CPTR(const Graph, graph)->generateDOT();
    return s.c_str();
}

const RDGraphData* RDGraph_GetData(const RDGraph* graph, RDGraphNode n) { return CPTR(const DataGraph, graph)->data(n); }
void RDGraph_SetDataUInt(RDGraph* graph, RDGraphNode n, uintptr_t val) { CPTR(DataGraph, graph)->setData(n, val); }
void RDGraph_SetDataInt(RDGraph* graph, RDGraphNode n, intptr_t val) { CPTR(DataGraph, graph)->setData(n, val); }
void RDGraph_SetDataString(RDGraph* graph, RDGraphNode n, const char* val) { CPTR(DataGraph, graph)->setData(n, val); }
void RDGraph_SetDataPointer(RDGraph* graph, RDGraphNode n, void* val) { CPTR(DataGraph, graph)->setData(n, val); }

const char* RDGraph_GetColor(const RDGraph* graph, const RDGraphEdge* e) { return CPTR(const StyledGraph, graph)->color(e); }
const char* RDGraph_GetLabel(const RDGraph* graph, const RDGraphEdge* e) { return CPTR(const StyledGraph, graph)->label(e); }
size_t RDGraph_GetRoutes(const RDGraph* graph, const RDGraphEdge* e, const RDGraphPoint** path) { return CPTR(const StyledGraph, graph)->routes(e, path); }
size_t RDGraph_GetArrow(const RDGraph* graph, const RDGraphEdge* e, const RDGraphPoint** path) { return CPTR(const StyledGraph, graph)->arrow(e, path); }
int RDGraph_GetAreaWidth(const RDGraph* graph) { return CPTR(const StyledGraph, graph)->areaWidth(); }
int RDGraph_GetAreaHeight(const RDGraph* graph) { return CPTR(const StyledGraph, graph)->areaHeight(); }
int RDGraph_GetX(const RDGraph* graph, RDGraphNode n) { return CPTR(const StyledGraph, graph)->x(n); }
int RDGraph_GetY(const RDGraph* graph, RDGraphNode n) { return CPTR(const StyledGraph, graph)->y(n); }
int RDGraph_GetWidth(const RDGraph* graph, RDGraphNode n) { return CPTR(const StyledGraph, graph)->width(n); }
int RDGraph_GetHeight(const RDGraph* graph, RDGraphNode n) { return CPTR(const StyledGraph, graph)->height(n); }
void RDGraph_SetColor(RDGraph* graph, const RDGraphEdge* e, const char* s) { CPTR(StyledGraph, graph)->color(e, s); }
void RDGraph_SetLabel(RDGraph* graph, const RDGraphEdge* e, const char* s) { CPTR(StyledGraph, graph)->label(e, s); }
void RDGraph_SetRoutes(RDGraph* graph, const RDGraphEdge* e, const RDGraphPoint* polyline, size_t n) { CPTR(StyledGraph, graph)->routes(e, polyline, n); }
void RDGraph_SetArrow(RDGraph* graph, const RDGraphEdge* e, const RDGraphPoint* polyline, size_t n) { CPTR(StyledGraph, graph)->arrow(e, polyline, n); }
void RDGraph_SetAreaWidth(RDGraph* graph, int w) { CPTR(StyledGraph, graph)->areaWidth(w); }
void RDGraph_SetAreaHeight(RDGraph* graph, int h) { CPTR(StyledGraph, graph)->areaHeight(h); }
void RDGraph_SetX(RDGraph* graph, RDGraphNode n, int px) { CPTR(StyledGraph, graph)->x(n, px); }
void RDGraph_SetY(RDGraph* graph, RDGraphNode n, int py) { CPTR(StyledGraph, graph)->y(n, py); }
void RDGraph_SetWidth(RDGraph* graph, RDGraphNode n, int w) { CPTR(StyledGraph, graph)->width(n, w); }
void RDGraph_SetHeight(RDGraph* graph, RDGraphNode n, int h) { CPTR(StyledGraph, graph)->height(n, h); }
