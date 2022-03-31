#include "emulate.h"
#include <rdcore/engine/algorithm/emulateresult.h>

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
void RDEmulateResult_AddBranchTable(RDEmulateResult* res, rd_address address, size_t size) { CPTR(EmulateResult, res)->addBranchTable(address, size); }
void RDEmulateResult_AddCall(RDEmulateResult* res, rd_address address) { CPTR(EmulateResult, res)->addCall(address); }
void RDEmulateResult_AddCallIndirect(RDEmulateResult* res) { CPTR(EmulateResult, res)->addCallIndirect();  }
void RDEmulateResult_AddCallUnresolved(RDEmulateResult* res) { CPTR(EmulateResult, res)->addCallUnresolved(); }
void RDEmulateResult_AddCallTable(RDEmulateResult* res, rd_address address, size_t size) { CPTR(EmulateResult, res)->addCallTable(address, size); }
void RDEmulateResult_AddReferenceSize(RDEmulateResult* res, rd_address address, size_t size) { CPTR(EmulateResult, res)->addReferenceSize(address, size); }
void RDEmulateResult_AddReference(RDEmulateResult* res, rd_address address) { CPTR(EmulateResult, res)->addReference(address); }
void RDEmulateResult_AddData(RDEmulateResult* res, rd_address address) { CPTR(EmulateResult, res)->addData(address); }
void RDEmulateResult_AddDataSize(RDEmulateResult* res, rd_address address, size_t size) { CPTR(EmulateResult, res)->addDataSize(address, size); }
void RDEmulateResult_AddString(RDEmulateResult* res, rd_address address) { CPTR(EmulateResult, res)->addString(address); }
void RDEmulateResult_AddStringSize(RDEmulateResult* res, rd_address address, size_t size) { CPTR(EmulateResult, res)->addStringSize(address, size); }
void RDEmulateResult_AddInvalid(RDEmulateResult* res, size_t size) { CPTR(EmulateResult, res)->addInvalid(size); }
void RDEmulateResult_AddTable(RDEmulateResult* res, rd_address address, size_t size) { CPTR(EmulateResult, res)->addTable(address, size); }
void RDEmulateResult_AddType(RDEmulateResult* res, rd_address address, const RDType* t) { if(t) CPTR(EmulateResult, res)->addType(address, CPTR(const Type, t)); }
void RDEmulateResult_AddTypeName(RDEmulateResult* res, rd_address address, const char* q) { if(q) CPTR(EmulateResult, res)->addTypeName(address, q); }
