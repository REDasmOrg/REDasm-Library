#pragma once

#include "graph.h"

DECLARE_HANDLE(RDFunctionBasicBlock);

struct RDDocumentItem;

// RDFunctionGraph
RD_API_EXPORT bool RDFunctionGraph_GetBasicBlock(const RDGraph* graph, RDGraphNode n, const RDFunctionBasicBlock** fbb);
RD_API_EXPORT bool RDFunctionGraph_Contains(const RDGraph* graph, address_t address);

// RDFunctionBasicBlock
RD_API_EXPORT const char* RDFunctionBasicBlock_GetStyle(const RDFunctionBasicBlock* fbb, RDGraphNode node);
RD_API_EXPORT size_t RDFunctionBasicBlock_ItemsCount(const RDFunctionBasicBlock* fbb);
RD_API_EXPORT size_t RDFunctionBasicBlock_GetStartIndex(const RDFunctionBasicBlock* fbb);
RD_API_EXPORT size_t RDFunctionBasicBlock_GetEndIndex(const RDFunctionBasicBlock* fbb);
RD_API_EXPORT address_t RDFunctionBasicBlock_GetStartAddress(const RDFunctionBasicBlock* fbb);
RD_API_EXPORT address_t RDFunctionBasicBlock_GetEndAddress(const RDFunctionBasicBlock* fbb);
RD_API_EXPORT bool RDFunctionBasicBlock_GetStartItem(const RDFunctionBasicBlock* fbb, RDDocumentItem* item);
RD_API_EXPORT bool RDFunctionBasicBlock_GetEndItem(const RDFunctionBasicBlock* fbb, RDDocumentItem* item);
RD_API_EXPORT bool RDFunctionBasicBlock_Contains(const RDFunctionBasicBlock* fbb, address_t address);
