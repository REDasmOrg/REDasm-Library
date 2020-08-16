#pragma once

#include "../../macros.h"
#include "../../types.h"

struct RDDisassembler;
struct RDBufferView;

DECLARE_HANDLE(RDEmulateResult);

RD_API_EXPORT RDDisassembler* RDEmulateResult_GetDisassembler(const RDEmulateResult* res);
RD_API_EXPORT const RDBufferView* RDEmulateResult_GetView(const RDEmulateResult* res);
RD_API_EXPORT rd_address RDEmulateResult_GetAddress(const RDEmulateResult* res);
RD_API_EXPORT void RDEmulateResult_SetSize(RDEmulateResult* res, size_t size);
RD_API_EXPORT void RDEmulateResult_SetDelaySlot(RDEmulateResult* res, size_t ds);
RD_API_EXPORT void RDEmulateResult_AddReturn(RDEmulateResult* res);
RD_API_EXPORT void RDEmulateResult_AddBranch(RDEmulateResult* res, rd_address address);
RD_API_EXPORT void RDEmulateResult_AddBranchUnresolved(RDEmulateResult* res);
RD_API_EXPORT void RDEmulateResult_AddBranchIndirect(RDEmulateResult* res);
RD_API_EXPORT void RDEmulateResult_AddBranchTrue(RDEmulateResult* res, rd_address address);
RD_API_EXPORT void RDEmulateResult_AddBranchFalse(RDEmulateResult* res, rd_address address);
RD_API_EXPORT void RDEmulateResult_AddCall(RDEmulateResult* res, rd_address address);
RD_API_EXPORT void RDEmulateResult_AddRef(RDEmulateResult* res, rd_address address);
