#pragma once

#include "macros.h"
#include "types.h"

DECLARE_HANDLE(RDNet);
DECLARE_HANDLE(RDNetNode);

RD_API_EXPORT const RDNetNode* RDNet_FindNode(const RDNet* net, rd_address address);
RD_API_EXPORT size_t RDNet_GetRefs(const RDNet* net, rd_address address, const rd_address** refs);

RD_API_EXPORT rd_address RDNetNode_GetNext(const RDNetNode* netnode);
RD_API_EXPORT rd_type RDNetNode_GetBranchType(const RDNetNode* netnode);
RD_API_EXPORT u64 RDNetNode_GetSysCall(const RDNetNode* netnode);
RD_API_EXPORT size_t RDNetNode_GetPrev(const RDNetNode* netnode, const rd_address** prev);
RD_API_EXPORT size_t RDNetNode_GetFrom(const RDNetNode* netnode, const rd_address** from);
RD_API_EXPORT size_t RDNetNode_GetBranchesTrue(const RDNetNode* netnode, const rd_address** branches);
RD_API_EXPORT size_t RDNetNode_GetBranchesFalse(const RDNetNode* netnode, const rd_address** branches);
RD_API_EXPORT size_t RDNetNode_GetCalls(const RDNetNode* netnode, const rd_address** calls);
