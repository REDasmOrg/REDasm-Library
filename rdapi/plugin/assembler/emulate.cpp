#include "emulate.h"
#include <rdcore/engine/algorithm/emulateresult.h>

RDDisassembler* RDEmulateResult_GetDisassembler(const RDEmulateResult* res) { return CPTR(RDDisassembler, CPTR(const EmulateResult, res)->disassembler()); }
const RDBufferView* RDEmulateResult_GetView(const RDEmulateResult* res) { return CPTR(const RDBufferView, CPTR(const EmulateResult, res)->view()); }
rd_address RDEmulateResult_GetAddress(const RDEmulateResult* res) { return CPTR(const EmulateResult, res)->address(); }
void RDEmulateResult_SetSize(RDEmulateResult* res, size_t size) { CPTR(EmulateResult, res)->setSize(size); }
void RDEmulateResult_SetDelaySlot(RDEmulateResult* res, size_t ds) { CPTR(EmulateResult, res)->setDelaySlot(ds); }
void RDEmulateResult_AddReturn(RDEmulateResult* res) { CPTR(EmulateResult, res)->addReturn(); }
void RDEmulateResult_AddBranch(RDEmulateResult* res, rd_address address) { CPTR(EmulateResult, res)->addBranch(address); }
void RDEmulateResult_AddBranchUnresolved(RDEmulateResult* res) { CPTR(EmulateResult, res)->addBranchUnresolved(); }
void RDEmulateResult_AddBranchIndirect(RDEmulateResult* res) { CPTR(EmulateResult, res)->addBranchIndirect(); }
void RDEmulateResult_AddBranchTrue(RDEmulateResult* res, rd_address address) { CPTR(EmulateResult, res)->addBranchTrue(address); }
void RDEmulateResult_AddBranchFalse(RDEmulateResult* res, rd_address address) { CPTR(EmulateResult, res)->addBranchFalse(address); }
void RDEmulateResult_AddCall(RDEmulateResult* res, rd_address address) { CPTR(EmulateResult, res)->addCall(address); }
void RDEmulateResult_AddRef(RDEmulateResult* res, rd_address address) { CPTR(EmulateResult, res)->addRef(address); }
