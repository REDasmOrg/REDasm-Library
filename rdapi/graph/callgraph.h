#pragma once

#include "graph.h"

RD_HANDLE(RDCallGraphItem);

struct RDNetNode;

// RDCallGraph
RD_API_EXPORT RDGraph* RDCallGraph_Create(RDContext* ctx);
RD_API_EXPORT bool RDCallGraph_Walk(RDGraph* graph, rd_address address);

// RDCallGraphItem
RD_API_EXPORT RDGraphNode RDCallGraphItem_GetNode(const RDCallGraphItem* item);
RD_API_EXPORT const RDNetNode* RDCallGraphItem_GetNetNode(const RDCallGraphItem* item);
