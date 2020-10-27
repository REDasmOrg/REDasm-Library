#include "ilfunction.h"
#include "../document/document.h"
#include "../plugin/assembler.h"
#include "../plugin/loader.h"
#include "../context.h"
#include <climits>

#define IL_EXPR_SIZE(size) (size ? size : (this->context()->bits() / CHAR_BIT))

ILFunction::ILFunction(Context* ctx): Object(ctx) { }

void ILFunction::insert(size_t idx, ILExpression* e)
{
    if(idx >= m_expressions.size())
    {
        this->append(e);
        return;
    }

    e = this->check(e);
    m_addresses[e] = m_currentaddress;
    m_expressions.emplace(m_expressions.begin() + idx, e);
}

const ILExpression* ILFunction::expression(size_t idx) const
{
    if(idx >= m_expressions.size()) return nullptr;
    return m_expressions[idx];
}

bool ILFunction::empty() const { return m_expressions.empty(); }
size_t ILFunction::size() const { return m_expressions.size(); }

bool ILFunction::generate(rd_address address, ILFunction* il)
{
    if(!il) return false;

    std::set<rd_address> path;
    if(!ILFunction::generatePath(address, il, path)) return false;

    auto* assembler = il->context()->assembler();
    auto* loader = il->context()->loader();
    RDBlock block;

    for(rd_address currentaddress : path)
    {
        const auto* blocks = il->context()->document()->blocks(currentaddress);
        if(!blocks->get(currentaddress, &block)) return false;

        RDBufferView view;
        if(!loader->view(currentaddress, BlockContainer::size(&block), &view)) return false;

        assembler->lift(currentaddress, &view, il);
    }

    return !il->empty();
}

void ILFunction::append(ILExpression* e)
{
    e = this->check(e);
    m_addresses[e] = m_currentaddress;
    m_expressions.push_back(e);
}

void ILFunction::setCurrentAddress(rd_address address) { m_currentaddress = address; }

bool ILFunction::getAddress(const ILExpression* e, rd_address* address) const
{
    if(!e) return false;

    auto it = m_addresses.find(e);
    if(it == m_addresses.end()) return false;

    if(address) *address = it->second;
    return true;
}

const ILExpression* ILFunction::first() const { return !m_expressions.empty() ? m_expressions.front() : nullptr; }
const ILExpression* ILFunction::last() const { return !m_expressions.empty() ? m_expressions.back() : nullptr; }
ILExpression* ILFunction::exprUNKNOWN() const { return this->expr(RDIL_Unknown); }
ILExpression* ILFunction::exprNOP() const { return this->expr(RDIL_Nop); }
ILExpression* ILFunction::exprPOP(ILExpression* e) const { return this->exprU(RDIL_Pop, 0, e); }
ILExpression* ILFunction::exprPUSH(ILExpression* e) const { return this->exprU(RDIL_Push, 0, e); }
ILExpression* ILFunction::exprVAR(size_t size, const char* name) const { return this->exprVALUE_P(RDIL_Var, IL_EXPR_SIZE(size), reinterpret_cast<uintptr_t>(name)); }
ILExpression* ILFunction::exprREG(size_t size, const char* reg) const { return this->exprVALUE_P(RDIL_Reg, IL_EXPR_SIZE(size), reinterpret_cast<uintptr_t>(reg)); }
ILExpression* ILFunction::exprCNST(size_t size, u64 value) const { return this->exprVALUE(RDIL_Cnst, size, value); }
ILExpression* ILFunction::exprJUMP(ILExpression* e) const { return this->exprU(RDIL_Goto, 0, e); }
ILExpression* ILFunction::exprCALL(ILExpression* e) const { return this->exprU(RDIL_Call, 0, e); }
ILExpression* ILFunction::exprRET(ILExpression* e) const { return this->exprU(RDIL_Ret, 0, e); }
ILExpression* ILFunction::exprMEM(ILExpression* e) const { return this->exprU(RDIL_Mem, 0, e); }
ILExpression* ILFunction::exprNOT(ILExpression* e) const { return this->exprU(RDIL_Not, 0, e); }
ILExpression* ILFunction::exprADD(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Add, 0, l, r); }
ILExpression* ILFunction::exprSUB(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Sub, 0, l, r); }
ILExpression* ILFunction::exprMUL(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Mul, 0, l, r); }
ILExpression* ILFunction::exprDIV(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Div, 0, l, r); }
ILExpression* ILFunction::exprAND(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Div, 0, l, r); }
ILExpression* ILFunction::exprOR(ILExpression* l, ILExpression* r)  const { return this->exprLR(RDIL_Or, 0, l, r);  }
ILExpression* ILFunction::exprXOR(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Xor, 0, l, r); }
ILExpression* ILFunction::exprCOPY(ILExpression* dst, ILExpression* src) const { return this->exprDS(RDIL_Copy, 0, dst, src); }
ILExpression* ILFunction::exprEQ(ILExpression* l, ILExpression* r)  const { return this->exprLR(RDIL_Eq, 0, l, r);  }
ILExpression* ILFunction::exprNE(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Ne, 0, l, r); }
ILExpression* ILFunction::exprLT(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Lt, 0, l, r); }
ILExpression* ILFunction::exprLE(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Le, 0, l, r); }
ILExpression* ILFunction::exprGT(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Gt, 0, l, r); }
ILExpression* ILFunction::exprGE(ILExpression* l, ILExpression* r) const { return this->exprLR(RDIL_Ge, 0, l, r); }
ILExpression* ILFunction::check(ILExpression* e) const { return e ? e : this->exprUNKNOWN(); }

ILExpression* ILFunction::exprIF(ILExpression* cond, ILExpression* t, ILExpression* f) const
{
    auto* expr = this->expr(RDIL_If, 0);
    expr->cond = cond;
    expr->t = this->check(t);
    expr->f = this->check(f);
    return expr;
}

ILExpression* ILFunction::exprVALUE_P(rd_type rdil, size_t size, uintptr_t value) const
{
    auto* expr = this->expr(rdil, size);
    expr->value = value;
    return expr;
}

ILExpression* ILFunction::exprVALUE(rd_type rdil, size_t size, u64 value) const
{
    auto* expr = this->expr(rdil, size);
    expr->u_value = value;
    return expr;
}

ILExpression* ILFunction::exprLR(rd_type rdil, size_t size, ILExpression* l, ILExpression* r) const
{
    auto* expr = this->expr(rdil, size);
    expr->left = this->check(l);
    expr->right = this->check(r);
    return expr;
}

ILExpression* ILFunction::exprDS(rd_type rdil, size_t size, ILExpression* dst, ILExpression* src) const
{
    auto* expr = this->expr(rdil, size);
    expr->dst = this->check(dst);
    expr->src = this->check(src);
    return expr;
}

ILExpression* ILFunction::exprU(rd_type rdil, size_t size, ILExpression* e) const
{
    auto* expr = this->expr(rdil, size);
    expr->u = this->check(e);
    return expr;
}

ILExpression* ILFunction::expr(rd_type rdil, size_t size) const
{
    auto& expr = m_pool.emplace_front(new ILExpression());
    expr->type = rdil;
    expr->size = size;
    expr->value = 0;
    return expr.get();
}

ILExpression* ILFunction::expr(rd_type rdil) const { return this->expr(rdil, 0); }

void ILFunction::generateBasicBlock(rd_address address, ILFunction* il, std::set<rd_address>& path)
{
    const auto* net = il->context()->net();
    auto* node = net->findNode(address);

    while(node)
    {
        path.insert(address);
        address = node->next;
        node = net->findNode(node->next);
    }
}

bool ILFunction::generatePath(rd_address address, ILFunction* il, std::set<rd_address>& path)
{
    auto* g = il->context()->document()->graph(address);

    if(!g)
    {
        ILFunction::generateBasicBlock(address, il, path); // It's not a function: try to generate a basic block
        return !path.empty();
    }

    const RDGraphNode* nodes = nullptr;
    size_t c = g->nodes(&nodes);

    for(size_t i = 0; i < c; i++)
    {
        const auto* data = g->data(nodes[i]);
        if(!data || !data->p_data) return false;

        const auto* fbb = reinterpret_cast<FunctionBasicBlock*>(data->p_data);
        ILFunction::generateBasicBlock(fbb->startaddress, il, path);
    }

    return true;
}
