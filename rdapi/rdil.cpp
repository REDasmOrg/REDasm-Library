#include "rdil.h"
#include <rdcore/builtin/graph/rdilgraph/rdilgraph.h>
#include <rdcore/rdil/ilfunction.h>
#include <rdcore/rdil/rdil.h>

RDGraph* RDILGraph_Create(RDContext* ctx, rd_address address)
{
    auto* g = new RDILGraph(CPTR(Context, ctx));
    g->build(address);
    return CPTR(RDGraph, g);
}

RDILExpression* RDILExpression_Create(RDContext* ctx, rd_address address) { return CPTR(RDILExpression, ILFunction::generateOne(CPTR(Context, ctx), address)); }
rd_type RDILExpression_Type(const RDILExpression* e) { return e ? CPTR(const ILExpression, e)->type : RDIL_Unknown; }
bool RDILExpression_Match(const RDILExpression* e, const char* m) { return e && m ? RDIL::match(CPTR(const ILExpression, e), m) : false; }
size_t RDILExpression_Extract(const RDILExpression* e, const RDILValue** values) { return e && values ? RDIL::extract(CPTR(const ILExpression, e), values) : 0; }

bool RDILExpression_GetValue(const RDILExpression* e, RDILValue* value)
{
    const ILExpression* expr = CPTR(const ILExpression, e);
    if(!expr || !RDIL::hasValue(expr)) return false;

    if(value)
    {
        value->type = expr->type;
        value->value = expr->value;
    }
    return true;
}

const char* RDILExpression_GetText(const RDILExpression* e)
{
    static std::string s;
    s = RDIL::getText(CPTR(const ILExpression, e));
    return s.c_str();
}

const char* RDILExpression_GetFormat(const RDILExpression* e)
{
    static std::string s;
    s = RDIL::getFormat(CPTR(const ILExpression, e));
    return s.c_str();
}

RDILFunction* RDILFunction_Create(RDContext* ctx, rd_address address)
{
    std::unique_ptr<ILFunction> il = std::make_unique<ILFunction>(CPTR(Context, ctx));
    if(!ILFunction::generate(address, il.get())) return nullptr;
    return CPTR(RDILFunction, il.release());
}

const RDILExpression* RDILFunction_GetExpression(const RDILFunction* rdilfunction, size_t idx) { return CPTR(const RDILExpression, CPTR(const ILFunction, rdilfunction)->expression(idx)); }
const RDILExpression* RDILFunction_GetFirstExpression(const RDILFunction* rdilfunction) { return CPTR(const RDILExpression, CPTR(const ILFunction, rdilfunction)->first()); }
const RDILExpression* RDILFunction_GetLastExpression(const RDILFunction* rdilfunction) { return CPTR(const RDILExpression, CPTR(const ILFunction, rdilfunction)->last()); }
size_t RDILFunction_Size(const RDILFunction* rdilfunction) { return CPTR(const ILFunction, rdilfunction)->size(); }
size_t RDILFunction_Extract(const RDILFunction* rdilfunction, const RDILValue** values) { return rdilfunction ? RDIL::extract(CPTR(const ILFunction, rdilfunction), values) : 0; }
bool RDILFunction_Match(const RDILFunction* rdilfunction, const char* m) { return rdilfunction && m ? RDIL::match(CPTR(const ILFunction, rdilfunction), m) : false; }
bool RDILFunction_Extract(const RDILFunction* rdilfunction, const char* m) { (void)rdilfunction; (void)m; return false; } //return rdilfunction && m ? RDIL::match(CPTR(const ILFunction, rdilfunction), m) : false; }
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
RDILExpression* RDILFunction_MOD(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprMOD(CPTR(ILExpression, l), CPTR(ILExpression, r))); }
RDILExpression* RDILFunction_AND(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprAND(CPTR(ILExpression, l), CPTR(ILExpression, r))); }
RDILExpression* RDILFunction_OR(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprOR(CPTR(ILExpression, l), CPTR(ILExpression, r))); }
RDILExpression* RDILFunction_XOR(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprXOR(CPTR(ILExpression, l), CPTR(ILExpression, r))); }
RDILExpression* RDILFunction_LSL(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprLSL(CPTR(ILExpression, l), CPTR(ILExpression, r))); }
RDILExpression* RDILFunction_LSR(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprLSR(CPTR(ILExpression, l), CPTR(ILExpression, r))); }
RDILExpression* RDILFunction_ASL(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprASL(CPTR(ILExpression, l), CPTR(ILExpression, r))); }
RDILExpression* RDILFunction_ASR(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprASR(CPTR(ILExpression, l), CPTR(ILExpression, r))); }
RDILExpression* RDILFunction_ROL(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprROL(CPTR(ILExpression, l), CPTR(ILExpression, r))); }
RDILExpression* RDILFunction_ROR(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprROR(CPTR(ILExpression, l), CPTR(ILExpression, r))); }
RDILExpression* RDILFunction_NOT(const RDILFunction* rdilfunction, RDILExpression* e) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprNOT(CPTR(ILExpression, e))); }
RDILExpression* RDILFunction_MEM(const RDILFunction* rdilfunction, RDILExpression* e) { return CPTR(RDILExpression, CPTR(const ILFunction, rdilfunction)->exprMEM(CPTR(ILExpression, e))); }
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
