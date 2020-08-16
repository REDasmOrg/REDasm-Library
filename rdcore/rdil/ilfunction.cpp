#include "ilfunction.h"
#include "../disassembler.h"
#include "../context.h"

ILFunction::ILFunction(const Disassembler* disassembler): m_disassembler(disassembler) { }

void ILFunction::insert(size_t idx, RDILExpression* e)
{
    if(idx >= m_expressions.size()) this->append(e);
    else if(e) m_expressions.emplace(m_expressions.begin() + idx, e);
}

const RDILExpression* ILFunction::expression(size_t idx) const
{
    if(idx >= m_expressions.size()) return nullptr;
    return m_expressions[idx].get();
}

bool ILFunction::empty() const { return m_expressions.empty(); }
size_t ILFunction::size() const { return m_expressions.size(); }

bool ILFunction::generate(const Disassembler* disassembler, rd_address address, ILFunction* il)
{
    auto* assembler = disassembler->assembler();
    auto* loader = disassembler->loader();
    const auto* net = disassembler->net();
    auto* node = net->findNode(address);
    RDBlock block;

    while(node)
    {
        const auto* blocks = disassembler->document()->blocks(address);
        if(!blocks->find(address, &block)) return false;

        RDBufferView view;
        if(!loader->view(address, BlockContainer::size(&block), &view)) return false;

        assembler->lift(address, &view, il);
        address = node->next;
        node = net->findNode(node->next);
    }

    return true;
}

void ILFunction::append(RDILExpression* e) { if(e) m_expressions.emplace_back(e); }
const RDILExpression* ILFunction::first() const { return !m_expressions.empty() ? m_expressions.front().get() : nullptr; }
const RDILExpression* ILFunction::last() const { return !m_expressions.empty() ? m_expressions.back().get() : nullptr; }
RDILExpression* ILFunction::exprUNKNOWN() const { return this->expr(RDIL_Unknown); }
RDILExpression* ILFunction::exprNOP() const { return this->expr(RDIL_Nop); }
RDILExpression* ILFunction::exprPOP(RDILExpression* e) const { return this->exprU(RDIL_Pop, 0, e); }
RDILExpression* ILFunction::exprPUSH(RDILExpression* e) const { return this->exprU(RDIL_Push, 0, e); }
RDILExpression* ILFunction::exprVAR(size_t size, const char* name) const { return this->exprVALUE_P(RDIL_Var, size, reinterpret_cast<uintptr_t>(name)); }
RDILExpression* ILFunction::exprREG(size_t size, const char* reg) const { return this->exprVALUE_P(RDIL_Reg, size, reinterpret_cast<uintptr_t>(reg)); }
RDILExpression* ILFunction::exprCNST(size_t size, u64 value) const { return this->exprVALUE(RDIL_Cnst, size, value); }
RDILExpression* ILFunction::exprADDR(rd_address addr) const { return this->exprVALUE(RDIL_Addr, m_disassembler->assembler()->bits() / CHAR_BIT, addr);  }
RDILExpression* ILFunction::exprJUMP(RDILExpression* e) const { return this->exprU(RDIL_Goto, 0, e); }
RDILExpression* ILFunction::exprCALL(RDILExpression* e) const { return this->exprU(RDIL_Call, 0, e); }
RDILExpression* ILFunction::exprRET(RDILExpression* e) const { return this->exprU(RDIL_Ret, 0, e); }
RDILExpression* ILFunction::exprLOAD(RDILExpression* memloc) const { return this->exprU(RDIL_Load, 0, memloc); }
RDILExpression* ILFunction::exprADD(RDILExpression* l, RDILExpression* r) const { return this->exprLR(RDIL_Add, 0, l, r); }
RDILExpression* ILFunction::exprSUB(RDILExpression* l, RDILExpression* r) const { return this->exprLR(RDIL_Sub, 0, l, r); }
RDILExpression* ILFunction::exprMUL(RDILExpression* l, RDILExpression* r) const { return this->exprLR(RDIL_Mul, 0, l, r); }
RDILExpression* ILFunction::exprDIV(RDILExpression* l, RDILExpression* r) const { return this->exprLR(RDIL_Div, 0, l, r); }
RDILExpression* ILFunction::exprAND(RDILExpression* l, RDILExpression* r) const { return this->exprLR(RDIL_Div, 0, l, r); }
RDILExpression* ILFunction::exprOR(RDILExpression* l, RDILExpression* r)  const { return this->exprLR(RDIL_Or, 0, l, r);  }
RDILExpression* ILFunction::exprXOR(RDILExpression* l, RDILExpression* r) const { return this->exprLR(RDIL_Xor, 0, l, r); }
RDILExpression* ILFunction::exprSTORE(RDILExpression* dst, RDILExpression* src) const { return this->exprDS(RDIL_Store, 0, dst, src); }
RDILExpression* ILFunction::exprCOPY(RDILExpression* dst, RDILExpression* src) const { return this->exprDS(RDIL_Copy, 0, dst, src); }
RDILExpression* ILFunction::exprEQ(RDILExpression* l, RDILExpression* r)  const { return this->exprLR(RDIL_Eq, 0, l, r);  }
RDILExpression* ILFunction::exprNE(RDILExpression* l, RDILExpression* r) const { return this->exprLR(RDIL_Ne, 0, l, r); }
RDILExpression* ILFunction::exprLT(RDILExpression* l, RDILExpression* r) const { return this->exprLR(RDIL_Lt, 0, l, r); }
RDILExpression* ILFunction::exprLE(RDILExpression* l, RDILExpression* r) const { return this->exprLR(RDIL_Le, 0, l, r); }
RDILExpression* ILFunction::exprGT(RDILExpression* l, RDILExpression* r) const { return this->exprLR(RDIL_Gt, 0, l, r); }
RDILExpression* ILFunction::exprGE(RDILExpression* l, RDILExpression* r) const { return this->exprLR(RDIL_Ge, 0, l, r); }

RDILExpression* ILFunction::exprIF(RDILExpression* cond, RDILExpression* t, RDILExpression* f) const
{
    auto* expr = this->expr(RDIL_If, 0);
    cond->parent = expr;
    t->parent = expr;
    f->parent = expr;
    expr->cond = cond;
    expr->t = t;
    expr->f = f;
    return expr;
}

RDILExpression* ILFunction::exprVALUE_P(rd_type rdil, size_t size, uintptr_t value) const
{
    auto* expr = this->expr(rdil, size);
    expr->value = value;
    return expr;
}

RDILExpression* ILFunction::exprVALUE(rd_type rdil, size_t size, u64 value) const
{
    auto* expr = this->expr(rdil, size);
    expr->u_value = value;
    return expr;
}

RDILExpression* ILFunction::exprLR(rd_type rdil, size_t size, RDILExpression* l, RDILExpression* r) const
{
    auto* expr = this->expr(rdil, size);
    l->parent = expr;
    r->parent = expr;
    expr->left = l;
    expr->right = r;
    return expr;
}

RDILExpression* ILFunction::exprDS(rd_type rdil, size_t size, RDILExpression* dst, RDILExpression* src) const
{
    auto* expr = this->expr(rdil, size);
    dst->parent = expr;
    src->parent = expr;
    expr->dst = dst;
    expr->src = src;
    return expr;
}

RDILExpression* ILFunction::exprU(rd_type rdil, size_t size, RDILExpression* e) const
{
    auto* expr = this->expr(rdil, size);
    e->parent = expr;
    expr->e = e;
    return expr;
}

RDILExpression* ILFunction::expr(rd_type rdil, size_t size) const
{
    RDILExpression* expr = new RDILExpression();
    expr->rdil = rdil;
    expr->size = size;
    return expr;
}

RDILExpression* ILFunction::expr(rd_type rdil) const { return this->expr(rdil, 0); }
