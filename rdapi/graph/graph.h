#pragma once

#include "../macros.h"
#include "../types.h"

struct RDContext;
RD_HANDLE(RDGraph);

typedef size_t RDGraphNode;

typedef struct RDGraphEdge {
    RDGraphNode source, target;
} RDGraphEdge;

typedef struct RDGraphPoint {
    int x, y;
} RDGraphPoint;

typedef union RDGraphData {
    uintptr_t nu_data;
    intptr_t ns_data;
    const char* s_data;
    void* p_data;
} RDGraphData;

// Graph
RD_API_EXPORT RDGraph* RDGraph_Create(RDContext* ctx);
RD_API_EXPORT const RDGraphEdge* RDGraph_GetEdge(const RDGraph* graph, RDGraphNode source, RDGraphNode target);
RD_API_EXPORT RDGraphNode RDGraph_PushNode(RDGraph* graph);
RD_API_EXPORT RDGraphNode RDGraph_GetRoot(const RDGraph* graph);
RD_API_EXPORT size_t RDGraph_GetOutgoing(const RDGraph* graph, RDGraphNode n, const RDGraphEdge** edges);
RD_API_EXPORT size_t RDGraph_GetIncoming(const RDGraph* graph, RDGraphNode n, const RDGraphEdge** edges);
RD_API_EXPORT size_t RDGraph_GetNodes(const RDGraph* graph, const RDGraphNode** nodes);
RD_API_EXPORT size_t RDGraph_GetEdges(const RDGraph* graph, const RDGraphEdge** edges);
RD_API_EXPORT bool RDGraph_ContainsEdge(const RDGraph* graph, RDGraphNode source, RDGraphNode target);
RD_API_EXPORT bool RDGraph_IsEmpty(const RDGraph* graph);
RD_API_EXPORT void RDGraph_PushEdge(RDGraph* graph, RDGraphNode source, RDGraphNode target);
RD_API_EXPORT void RDGraph_SetRoot(RDGraph* graph, RDGraphNode n);
RD_API_EXPORT void RDGraph_RemoveEdge(RDGraph* graph, const RDGraphEdge* e);
RD_API_EXPORT void RDGraph_RemoveNode(RDGraph* graph, RDGraphNode n);
RD_API_EXPORT u32 RDGraph_Hash(const RDGraph* graph);
RD_API_EXPORT const char* RDGraph_GenerateDOT(const RDGraph* graph);

// Data
RD_API_EXPORT const RDGraphData* RDGraph_GetData(const RDGraph* graph, RDGraphNode n);
RD_API_EXPORT void RDGraph_SetDataUInt(RDGraph* graph, RDGraphNode n, uintptr_t val);
RD_API_EXPORT void RDGraph_SetDataInt(RDGraph* graph, RDGraphNode n, intptr_t val);
RD_API_EXPORT void RDGraph_SetDataString(RDGraph* graph, RDGraphNode n, const char* val);
RD_API_EXPORT void RDGraph_SetDataPointer(RDGraph* graph, RDGraphNode n, void* val);

// Styling
RD_API_EXPORT const char* RDGraph_GetColor(const RDGraph* graph, const RDGraphEdge* e);
RD_API_EXPORT const char* RDGraph_GetLabel(const RDGraph* graph, const RDGraphEdge* e);
RD_API_EXPORT size_t RDGraph_GetRoutes(const RDGraph* graph, const RDGraphEdge* e, const RDGraphPoint** path);
RD_API_EXPORT size_t RDGraph_GetArrow(const RDGraph* graph, const RDGraphEdge* e, const RDGraphPoint** path);
RD_API_EXPORT int RDGraph_GetAreaWidth(const RDGraph* graph);
RD_API_EXPORT int RDGraph_GetAreaHeight(const RDGraph* graph);
RD_API_EXPORT int RDGraph_GetX(const RDGraph* graph, RDGraphNode n);
RD_API_EXPORT int RDGraph_GetY(const RDGraph* graph, RDGraphNode n);
RD_API_EXPORT int RDGraph_GetWidth(const RDGraph* graph, RDGraphNode n);
RD_API_EXPORT int RDGraph_GetHeight(const RDGraph* graph, RDGraphNode n);
RD_API_EXPORT void RDGraph_SetColor(RDGraph* graph, const RDGraphEdge* e, const char* s);
RD_API_EXPORT void RDGraph_SetLabel(RDGraph* graph, const RDGraphEdge* e, const char* s);
RD_API_EXPORT void RDGraph_SetRoutes(RDGraph* graph, const RDGraphEdge* e, const RDGraphPoint* polyline, size_t n);
RD_API_EXPORT void RDGraph_SetArrow(RDGraph* graph, const RDGraphEdge* e, const RDGraphPoint* polyline, size_t n);
RD_API_EXPORT void RDGraph_SetAreaWidth(RDGraph* graph, int w);
RD_API_EXPORT void RDGraph_SetAreaHeight(RDGraph* graph, int h);
RD_API_EXPORT void RDGraph_SetX(RDGraph* graph, RDGraphNode n, int px);
RD_API_EXPORT void RDGraph_SetY(RDGraph* graph, RDGraphNode n, int py);
RD_API_EXPORT void RDGraph_SetWidth(RDGraph* graph, RDGraphNode n, int w);
RD_API_EXPORT void RDGraph_SetHeight(RDGraph* graph, RDGraphNode n, int h);

#ifdef __cplusplus
#include <utility>
#include <functional>

namespace std {

template<>
struct hash<RDGraphEdge> {
    size_t operator()(const RDGraphEdge& edge) const {
        return edge.source ^ edge.target;
    }
};

template<>
struct equal_to<RDGraphEdge> {
    bool operator()(const RDGraphEdge& edge1, const RDGraphEdge& edge2) const {
        return (edge1.source == edge2.source) &&
               (edge1.target == edge2.target);
    }
};

} // namespace std
#endif
