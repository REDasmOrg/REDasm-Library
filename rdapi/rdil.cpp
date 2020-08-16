#include "rdil.h"
#include <rdcore/builtin/graph/rdilgraph.h>
#include <rdcore/rdil/ilfunction.h>
#include <rdcore/disassembler.h>

RDGraph* RDILGraph_Create(const RDDisassembler* disassembler, rd_address address)
{
    auto* g = new RDILGraph(CPTR(const Disassembler, disassembler));
    g->build(address);
    return CPTR(RDGraph, g);
}

const RDExpression* RDILFunction_GetExpression(const RDILFunction* rdilfunction, size_t idx) { return CPTR(const RDExpression, CPTR(const ILFunction, rdilfunction)->expression(idx)); }
const RDExpression* RDILFunction_GetFirstExpression(const RDILFunction* rdilfunction) { return CPTR(const RDExpression, CPTR(const ILFunction, rdilfunction)->first()); }
const RDExpression* RDILFunction_GetLastExpression(const RDILFunction* rdilfunction) { return CPTR(const RDExpression, CPTR(const ILFunction, rdilfunction)->last()); }
size_t RDILFunction_Size(const RDILFunction* rdilfunction) { return CPTR(const ILFunction, rdilfunction)->size(); }
void RDILFunction_Insert(RDILFunction* rdilfunction, size_t idx, RDExpression* expression) { CPTR(ILFunction, rdilfunction)->insert(idx, CPTR(RDILExpression, expression)); }
void RDILFunction_Append(RDILFunction* rdilfunction, RDExpression* expression) { CPTR(ILFunction, rdilfunction)->append(CPTR(RDILExpression, expression)); }
RDExpression* RDILFunction_UNKNOWN(const RDILFunction* rdilfunction) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprUNKNOWN()); }
RDExpression* RDILFunction_NOP(const RDILFunction* rdilfunction) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprNOP());  }
RDExpression* RDILFunction_POP(const RDILFunction* rdilfunction, RDExpression* e) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprPOP(CPTR(RDILExpression, e))); }
RDExpression* RDILFunction_PUSH(const RDILFunction* rdilfunction, RDExpression* e) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprPUSH(CPTR(RDILExpression, e))); }
RDExpression* RDILFunction_VAR(const RDILFunction* rdilfunction, size_t size, const char* name) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprVAR(size, name)); }
RDExpression* RDILFunction_REG(const RDILFunction* rdilfunction, size_t size, const char* reg) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprREG(size, reg)); }
RDExpression* RDILFunction_CNST(const RDILFunction* rdilfunction, size_t size, u64 value) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprCNST(size, value)); }
RDExpression* RDILFunction_ADDR(const RDILFunction* rdilfunction, rd_address address) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprADDR(address)); }
RDExpression* RDILFunction_ADD(const RDILFunction* rdilfunction, RDExpression* l, RDExpression* r) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprADD(CPTR(RDILExpression, l), CPTR(RDILExpression, r))); }
RDExpression* RDILFunction_SUB(const RDILFunction* rdilfunction, RDExpression* l, RDExpression* r) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprSUB(CPTR(RDILExpression, l), CPTR(RDILExpression, r))); }
RDExpression* RDILFunction_MUL(const RDILFunction* rdilfunction, RDExpression* l, RDExpression* r) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprMUL(CPTR(RDILExpression, l), CPTR(RDILExpression, r))); }
RDExpression* RDILFunction_DIV(const RDILFunction* rdilfunction, RDExpression* l, RDExpression* r) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprDIV(CPTR(RDILExpression, l), CPTR(RDILExpression, r))); }
RDExpression* RDILFunction_AND(const RDILFunction* rdilfunction, RDExpression* l, RDExpression* r) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprAND(CPTR(RDILExpression, l), CPTR(RDILExpression, r))); }
RDExpression* RDILFunction_OR(const RDILFunction* rdilfunction, RDExpression* l, RDExpression* r) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprOR(CPTR(RDILExpression, l), CPTR(RDILExpression, r))); }
RDExpression* RDILFunction_XOR(const RDILFunction* rdilfunction, RDExpression* l, RDExpression* r) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprXOR(CPTR(RDILExpression, l), CPTR(RDILExpression, r))); }
RDExpression* RDILFunction_LOAD(const RDILFunction* rdilfunction, RDExpression* memloc) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprLOAD(CPTR(RDILExpression, memloc))); }
RDExpression* RDILFunction_STORE(const RDILFunction* rdilfunction, RDExpression* dst, RDExpression* src) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprSTORE(CPTR(RDILExpression, dst), CPTR(RDILExpression, src))); }
RDExpression* RDILFunction_COPY(const RDILFunction* rdilfunction, RDExpression* dst, RDExpression* src) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprCOPY(CPTR(RDILExpression, dst), CPTR(RDILExpression, src))); }
RDExpression* RDILFunction_GOTO(const RDILFunction* rdilfunction, RDExpression* e) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprJUMP(CPTR(RDILExpression, e))); }
RDExpression* RDILFunction_CALL(const RDILFunction* rdilfunction, RDExpression* e) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprCALL(CPTR(RDILExpression, e))); }
RDExpression* RDILFunction_RET(const RDILFunction* rdilfunction, RDExpression* e) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprRET(CPTR(RDILExpression, e))); }
RDExpression* RDILFunction_IF(const RDILFunction* rdilfunction, RDExpression* cond, RDExpression* t, RDExpression* f) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprIF(CPTR(RDILExpression, cond), CPTR(RDILExpression, t), CPTR(RDILExpression, f))); }
RDExpression* RDILFunction_EQ(const RDILFunction* rdilfunction, RDExpression* l, RDExpression* r) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprEQ(CPTR(RDILExpression, l), CPTR(RDILExpression, r))); }
RDExpression* RDILFunction_NE(const RDILFunction* rdilfunction, RDExpression* l, RDExpression* r) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprNE(CPTR(RDILExpression, l), CPTR(RDILExpression, r))); }
RDExpression* RDILFunction_LT(const RDILFunction* rdilfunction, RDExpression* l, RDExpression* r) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprLT(CPTR(RDILExpression, l), CPTR(RDILExpression, r))); }
RDExpression* RDILFunction_LE(const RDILFunction* rdilfunction, RDExpression* l, RDExpression* r) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprLE(CPTR(RDILExpression, l), CPTR(RDILExpression, r))); }
RDExpression* RDILFunction_GT(const RDILFunction* rdilfunction, RDExpression* l, RDExpression* r) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprGT(CPTR(RDILExpression, l), CPTR(RDILExpression, r))); }
RDExpression* RDILFunction_GE(const RDILFunction* rdilfunction, RDExpression* l, RDExpression* r) { return CPTR(RDExpression, CPTR(const ILFunction, rdilfunction)->exprGE(CPTR(RDILExpression, l), CPTR(RDILExpression, r))); }
