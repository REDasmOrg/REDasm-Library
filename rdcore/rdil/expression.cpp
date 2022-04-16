#include "expression.h"

ILExpression* ILExpression::clone(const ILExpression* e)
{
    if(!e) return nullptr;
    auto* ne = new ILExpression();
    ne->type = e->type;
    ne->size = e->size;
    ne->value = e->value;

    if(e->n1)
    {
        auto it = ne->children.emplace(ILExpression::clone(e->n1));
        ne->n1 = it.first->get();
    }

    if(e->n2)
    {
        auto it = ne->children.emplace(ILExpression::clone(e->n2));
        ne->n2 = it.first->get();
    }

    if(e->n3)
    {
        auto it = ne->children.emplace(ILExpression::clone(e->n3));
        ne->n3 = it.first->get();
    }

    return ne;
}

ILExpressionTree::ILExpressionTree(): Object() { }
ILExpressionTree::ILExpressionTree(Context* ctx): Object(ctx) { }
ILExpression* ILExpressionTree::exprUNKNOWN() const { return this->expr(RDIL_Unknown); }
ILExpression* ILExpressionTree::exprNOP() const { return this->expr(RDIL_Nop); }
ILExpression* ILExpressionTree::exprPOP(ILExpression* e) const { return this->exprU(RDIL_Pop, 0, e); }
ILExpression* ILExpressionTree::exprPUSH(ILExpression* e) const { return this->exprU(RDIL_Push, 0, e); }
ILExpression* ILExpressionTree::exprVAR(size_t size, const char* name) const { return this->exprVALUE_P(RDIL_Var, size, reinterpret_cast<uintptr_t>(name)); }
ILExpression* ILExpressionTree::exprREG(size_t size, const char* reg) const { return this->exprVALUE_P(RDIL_Reg, size, reinterpret_cast<uintptr_t>(reg)); }
ILExpression* ILExpressionTree::exprCNST(size_t size, u64 value) const { return this->exprVALUE(RDIL_Cnst, size, value); }
ILExpression* ILExpressionTree::exprJUMP(ILExpression* e) const { return this->exprU(RDIL_Goto, 0, e); }
ILExpression* ILExpressionTree::exprCALL(ILExpression* e) const { return this->exprU(RDIL_Call, 0, e); }
ILExpression* ILExpressionTree::exprRET(ILExpression* e) const { return this->exprU(RDIL_Ret, 0, e); }
ILExpression* ILExpressionTree::exprMEM(ILExpression* e) const { return this->exprU(RDIL_Mem, 0, e); }
ILExpression* ILExpressionTree::exprNOT(ILExpression* e) const { return this->exprU(RDIL_Not, 0, e); }
ILExpression* ILExpressionTree::exprADD(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Add, 0, l, r); }
ILExpression* ILExpressionTree::exprSUB(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Sub, 0, l, r); }
ILExpression* ILExpressionTree::exprMUL(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Mul, 0, l, r); }
ILExpression* ILExpressionTree::exprDIV(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Div, 0, l, r); }
ILExpression* ILExpressionTree::exprMOD(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Mod, 0, l, r); }
ILExpression* ILExpressionTree::exprAND(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Div, 0, l, r); }
ILExpression* ILExpressionTree::exprOR(ILExpression* l, ILExpression* r)  const { return this->exprLR(RDIL_Or, 0, l, r);  }
ILExpression* ILExpressionTree::exprXOR(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Xor, 0, l, r); }
ILExpression* ILExpressionTree::exprLSL(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Lsl, 0, l, r); }
ILExpression* ILExpressionTree::exprLSR(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Lsr, 0, l, r); }
ILExpression* ILExpressionTree::exprASL(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Asl, 0, l, r); }
ILExpression* ILExpressionTree::exprASR(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Asr, 0, l, r); }
ILExpression* ILExpressionTree::exprROL(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Rol, 0, l, r); }
ILExpression* ILExpressionTree::exprROR(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Ror, 0, l, r); }
ILExpression* ILExpressionTree::exprCOPY(ILExpression* dst, ILExpression* src) const { return this->exprDS(RDIL_Copy, 0, dst, src); }
ILExpression* ILExpressionTree::exprEQ(ILExpression* l, ILExpression* r)  const { return this->exprLR(RDIL_Eq, 0, l, r);  }
ILExpression* ILExpressionTree::exprNE(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Ne, 0, l, r); }
ILExpression* ILExpressionTree::exprLT(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Lt, 0, l, r); }
ILExpression* ILExpressionTree::exprLE(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Le, 0, l, r); }
ILExpression* ILExpressionTree::exprGT(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Gt, 0, l, r); }
ILExpression* ILExpressionTree::exprGE(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Ge, 0, l, r); }
ILExpression* ILExpressionTree::exprINT(ILExpression* e) const { return this->exprU(RDIL_Int, 0, e); }

ILExpression* ILExpressionTree::check(ILExpression* e) const { return e ? e : this->exprUNKNOWN(); }

ILExpression* ILExpressionTree::exprIF(ILExpression* cond, ILExpression* t, ILExpression* f) const
{
    auto* expr = this->expr(RDIL_If, 0);
    expr->cond = cond;
    expr->t = this->check(t);
    expr->f = this->check(f);
    return expr;
}

ILExpression* ILExpressionTree::exprVALUE_P(rd_type rdil, size_t size, uintptr_t value) const
{
    auto* expr = this->expr(rdil, size);
    expr->value = value;
    return expr;
}

ILExpression* ILExpressionTree::exprVALUE(rd_type rdil, size_t size, u64 value) const
{
    auto* expr = this->expr(rdil, size);
    expr->u_value = value;
    return expr;
}

ILExpression* ILExpressionTree::exprLR(rd_type rdil, size_t size, ILExpression* l, ILExpression* r) const
{
    auto* expr = this->expr(rdil, size);
    expr->left = this->check(l);
    expr->right = this->check(r);
    return expr;
}

ILExpression* ILExpressionTree::exprDS(rd_type rdil, size_t size, ILExpression* dst, ILExpression* src) const
{
    auto* expr = this->expr(rdil, size);
    expr->dst = this->check(dst);
    expr->src = this->check(src);
    return expr;
}

ILExpression* ILExpressionTree::exprU(rd_type rdil, size_t size, ILExpression* e) const
{
    auto* expr = this->expr(rdil, size);
    expr->u = this->check(e);
    return expr;
}

ILExpression* ILExpressionTree::expr(rd_type rdil, size_t size) const
{
    auto& expr = m_pool.emplace_front(new ILExpression());
    expr->type = rdil;
    expr->size = size;
    expr->value = 0;
    return expr.get();
}

ILExpression* ILExpressionTree::expr(rd_type rdil) const { return this->expr(rdil, 0); }
