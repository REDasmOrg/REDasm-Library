#include "net.h"
#include <rdcore/document/documentnet.h>

const RDNetNode* RDNet_FindNode(const RDNet* net, rd_address address) { return CPTR(const RDNetNode, CPTR(const DocumentNet, net)->findNode(address)); }
const RDNetNode* RDNet_GetPrevNode(const RDNet* net, const RDNetNode* netnode) { return CPTR(const RDNetNode, CPTR(const DocumentNet, net)->prevNode(CPTR(const DocumentNetNode, netnode))); }
const RDNetNode* RDNet_GetNextNode(const RDNet* net, const RDNetNode* netnode) { return CPTR(const RDNetNode, CPTR(const DocumentNet, net)->nextNode(CPTR(const DocumentNetNode, netnode))); }
size_t RDNet_GetReferences(const RDNet* net, rd_address address, const rd_address** refs) { return CPTR(const DocumentNet, net)->getReferences(address, refs); }

rd_address RDNetNode_GetAddress(const RDNetNode* netnode) { return CPTR(const DocumentNetNode, netnode)->address; }
rd_type RDNetNode_GetBranchType(const RDNetNode* netnode) { return CPTR(const DocumentNetNode, netnode)->branchtype; }
u64 RDNetNode_GetSysCall(const RDNetNode* netnode) { return CPTR(const DocumentNetNode, netnode)->syscall; }
rd_address RDNetNode_GetNext(const RDNetNode* netnode) { return CPTR(const DocumentNetNode, netnode)->next; }
size_t RDNetNode_GetFrom(const RDNetNode* netnode, const rd_address** from) { return CPTR(const DocumentNetNode, netnode)->from.data(from); }
size_t RDNetNode_GetPrev(const RDNetNode* netnode, const rd_address** prev) { return CPTR(const DocumentNetNode, netnode)->prev.data(prev); }
size_t RDNetNode_GetBranchesTrue(const RDNetNode* netnode, const rd_address** branches) { return CPTR(const DocumentNetNode, netnode)->branchestrue.data(branches); }
size_t RDNetNode_GetBranchesFalse(const RDNetNode* netnode, const rd_address** branches) { return CPTR(const DocumentNetNode, netnode)->branchesfalse.data(branches); }
size_t RDNetNode_GetCalls(const RDNetNode* netnode, const rd_address** calls) { return CPTR(const DocumentNetNode, netnode)->calls.data(calls); }
bool RDNetNode_IsConditional(const RDNetNode* netnode) { return DocumentNet::isConditional(CPTR(const DocumentNetNode, netnode)); }
bool RDNetNode_IsBranch(const RDNetNode* netnode) { return DocumentNet::isBranch(CPTR(const DocumentNetNode, netnode)); }
bool RDNetNode_IsCall(const RDNetNode* netnode) { return DocumentNet::isCall(CPTR(const DocumentNetNode, netnode)); }
