#include "rdil.h"
#include <rdcore/builtin/graph/rdilgraph.h>
#include <rdcore/rdil/ilfunction.h>
#include <rdcore/rdil/rdil.h>
#include <rdcore/disassembler.h>

RDGraph* RDILGraph_Create(const RDDisassembler* disassembler, rd_address address)
{
    auto* g = new RDILGraph(CPTR(const Disassembler, disassembler));
    g->build(address);
    return CPTR(RDGraph, g);
}

rd_type RDILExpression_Type(const RDILExpression* e) { return e ? CPTR(const ILExpression, e)->type : RDIL_Unknown; }
bool RDILExpression_Match(const RDILExpression* e, const char* m) { return e ? RDIL::match(CPTR(const ILExpression, e), m) : false; }
const RDILExpression* RDILExpression_GetN1(const RDILExpression* e) { return CPTR(const RDILExpression, CPTR(const ILExpression, e)->n1); }
const RDILExpression* RDILExpression_GetN2(const RDILExpression* e) { return CPTR(const RDILExpression, CPTR(const ILExpression, e)->n2); }
const RDILExpression* RDILExpression_GetN3(const RDILExpression* e) { return CPTR(const RDILExpression, CPTR(const ILExpression, e)->n3); }
const RDILExpression* RDILExpression_GetE(const RDILExpression* e) { return CPTR(const RDILExpression, CPTR(const ILExpression, e)->e); }
const RDILExpression* RDILExpression_GetCond(const RDILExpression* e) { return CPTR(const RDILExpression, CPTR(const ILExpression, e)->cond); }
const RDILExpression* RDILExpression_GetDst(const RDILExpression* e) { return CPTR(const RDILExpression, CPTR(const ILExpression, e)->dst); }
const RDILExpression* RDILExpression_GetLeft(const RDILExpression* e) { return CPTR(const RDILExpression, CPTR(const ILExpression, e)->left); }
const RDILExpression* RDILExpression_GetT(const RDILExpression* e) { return CPTR(const RDILExpression, CPTR(const ILExpression, e)->t); }
const RDILExpression* RDILExpression_GetSrc(const RDILExpression* e) { return CPTR(const RDILExpression, CPTR(const ILExpression, e)->src); }
const RDILExpression* RDILExpression_GetRight(const RDILExpression* e) { return CPTR(const RDILExpression, CPTR(const ILExpression, e)->right); }
const RDILExpression* RDILExpression_GetF(const RDILExpression* e) { return CPTR(const RDILExpression, CPTR(const ILExpression, e)->f); }

bool RDILExpression_GetValue(const RDILExpression* e, RDILValue* value)
{
    const ILExpression* expr = CPTR(const ILExpression, e);
    if(!expr || !RDIL::hasValue(expr)) return false;

    if(value) value->value = expr->value;
    return true;
}

RDILFunction* RDILFunction_Generate(const RDDisassembler* disassembler, rd_address address)
{
    std::unique_ptr<ILFunction> il = std::make_unique<ILFunction>(CPTR(const Disassembler, disassembler));
    if(!ILFunction::generate(address, il.get())) return nullptr;
    return CPTR(RDILFunction, il.release());
}

const RDILExpression* RDILFunction_GetExpression(const RDILFunction* rdilfunction, size_t idx) { return CPTR(const RDILExpression, CPTR(const ILFunction, rdilfunction)->expression(idx)); }
const RDILExpression* RDILFunction_GetFirstExpression(const RDILFunction* rdilfunction) { return CPTR(const RDILExpression, CPTR(const ILFunction, rdilfunction)->first()); }
const RDILExpression* RDILFunction_GetLastExpression(const RDILFunction* rdilfunction) { return CPTR(const RDILExpression, CPTR(const ILFunction, rdilfunction)->last()); }
size_t RDILFunction_Size(const RDILFunction* rdilfunction) { return CPTR(const ILFunction, rdilfunction)->size(); }
bool RDILFunction_GetAddress(const RDILFunction* rdilfunction, const RDILExpression* e, rd_address* address) { return CPTR(const ILFunction, rdilfunction)->getAddress(CPTR(const ILExpression, e), address); }
void RDILFunction_Insert(RDILFunction* rdilfunction, size_t idx, RDILExpression* expression) { CPTR(ILFunction, rdilfunction)->insert(idx, CPTR(ILExpression, expression)); }
void RDILFunction_Append(RDILFunction* rdilfunction, RDILExpression* expression) { CPTR(ILFunction, rdilfunction)->append(CPTR(ILExpression, expression)); }
RDILExpression* RDILFunction_UNKNOWN(const RDILFunction* rdilfunction) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprUNKNOWN()); }
RDILExpression* RDILFunction_NOP(const RDILFunction* rdilfunction) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprNOP());  }
RDILExpression* RDILFunction_POP(const RDILFunction* rdilfunction, RDILExpression* e) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprPOP(CPTR(ILExpression, e))); }
RDILExpression* RDILFunction_PUSH(const RDILFunction* rdilfunction, RDILExpression* e) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprPUSH(CPTR(ILExpression, e))); }
RDILExpression* RDILFunction_VAR(const RDILFunction* rdilfunction, size_t size, const char* name) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprVAR(size, name)); }
RDILExpression* RDILFunction_REG(const RDILFunction* rdilfunction, size_t size, const char* reg) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprREG(size, reg)); }
RDILExpression* RDILFunction_CNST(const RDILFunction* rdilfunction, size_t size, u64 value) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprCNST(size, value)); }
RDILExpression* RDILFunction_ADD(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprADD(CPTR(ILExpression, l), CPTR(ILExpression, r))); }
RDILExpression* RDILFunction_SUB(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprSUB(CPTR(ILExpression, l), CPTR(ILExpression, r))); }
RDILExpression* RDILFunction_MUL(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprMUL(CPTR(ILExpression, l), CPTR(ILExpression, r))); }
RDILExpression* RDILFunction_DIV(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprDIV(CPTR(ILExpression, l), CPTR(ILExpression, r))); }
RDILExpression* RDILFunction_AND(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprAND(CPTR(ILExpression, l), CPTR(ILExpression, r))); }
RDILExpression* RDILFunction_OR(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprOR(CPTR(ILExpression, l), CPTR(ILExpression, r))); }
RDILExpression* RDILFunction_XOR(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprXOR(CPTR(ILExpression, l), CPTR(ILExpression, r))); }
RDILExpression* RDILFunction_LOAD(const RDILFunction* rdilfunction, RDILExpression* memloc) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprLOAD(CPTR(ILExpression, memloc))); }
RDILExpression* RDILFunction_STORE(const RDILFunction* rdilfunction, RDILExpression* dst, RDILExpression* src) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprSTORE(CPTR(ILExpression, dst), CPTR(ILExpression, src))); }
RDILExpression* RDILFunction_COPY(const RDILFunction* rdilfunction, RDILExpression* dst, RDILExpression* src) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprCOPY(CPTR(ILExpression, dst), CPTR(ILExpression, src))); }
RDILExpression* RDILFunction_GOTO(const RDILFunction* rdilfunction, RDILExpression* e) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprJUMP(CPTR(ILExpression, e))); }
RDILExpression* RDILFunction_CALL(const RDILFunction* rdilfunction, RDILExpression* e) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprCALL(CPTR(ILExpression, e))); }
RDILExpression* RDILFunction_RET(const RDILFunction* rdilfunction, RDILExpression* e) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprRET(CPTR(ILExpression, e))); }
RDILExpression* RDILFunction_IF(const RDILFunction* rdilfunction, RDILExpression* cond, RDILExpression* t, RDILExpression* f) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprIF(CPTR(ILExpression, cond), CPTR(ILExpression, t), CPTR(ILExpression, f))); }
RDILExpression* RDILFunction_EQ(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprEQ(CPTR(ILExpression, l), CPTR(ILExpression, r))); }
RDILExpression* RDILFunction_NE(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprNE(CPTR(ILExpression, l), CPTR(ILExpression, r))); }
RDILExpression* RDILFunction_LT(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprLT(CPTR(ILExpression, l), CPTR(ILExpression, r))); }
RDILExpression* RDILFunction_LE(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprLE(CPTR(ILExpression, l), CPTR(ILExpression, r))); }
RDILExpression* RDILFunction_GT(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprGT(CPTR(ILExpression, l), CPTR(ILExpression, r))); }
RDILExpression* RDILFunction_GE(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprGE(CPTR(ILExpression, l), CPTR(ILExpression, r))); }
