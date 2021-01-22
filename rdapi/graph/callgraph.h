#pragma once

#include "graph.h"

RD_HANDLE(RDCallGraphItem);

struct RDILExpression;

// RDCallGraph
RD_API_EXPORT RDGraph* RDCallGraph_Create(RDContext* ctx);
RD_API_EXPORT bool RDCallGraph_Walk(RDGraph* graph, rd_address address);

// RDCallGraphItem
RD_API_EXPORT RDGraphNode RDCallGraphItem_GetNode(const RDCallGraphItem* item);
RD_API_EXPORT rd_address RDCallGraphItem_GetAddress(const RDCallGraphItem* item);
RD_API_EXPORT size_t RDCallGraphItem_GetSize(const RDCallGraphItem* item);
RD_API_EXPORT const RDILExpression* RDCallGraphItem_GetCall(const RDCallGraphItem* item, size_t idx);
