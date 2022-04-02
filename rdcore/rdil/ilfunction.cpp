#include "ilfunction.h"
#include "../document/document.h"
#include "../plugin/assembler.h"
#include "../plugin/loader.h"
#include "../context.h"
#include <climits>

#define IL_EXPR_SIZE(size) (size ? size : (this->context()->bits() / CHAR_BIT))

ILFunction::ILFunction(Context* ctx): ILExpressionTree(ctx) { }

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
    return m_expressions.at(idx);
}

bool ILFunction::empty() const { return m_expressions.empty(); }
size_t ILFunction::size() const { return m_expressions.size(); }

bool ILFunction::generate(rd_address address, ILFunction* il)
{
    if(!il) return false;

    std::set<rd_address> path;
    if(!ILFunction::generatePath(address, il, path)) return false;

    auto* assembler = il->context()->assembler();
    auto& document = il->context()->document();

    for(rd_address currentaddress : path)
    {
        RDBufferView view;
        if(!document->getBlockView(currentaddress, &view)) return false;
        assembler->lift(currentaddress, &view, il);
    }

    return !il->empty();
}

ILExpression* ILFunction::generateOne(Context* ctx, rd_address address)
{
    RDBufferView view;
    if(!ctx->document()->getBlockView(address, &view)) return nullptr;

    ILFunction il(ctx);
    ctx->assembler()->lift(address, &view, &il);
    return il.empty() ? nullptr : ILExpression::clone(il.first());
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

ILFunction::ExpressionList::iterator ILFunction::begin() { return m_expressions.begin(); }
ILFunction::ExpressionList::iterator ILFunction::end() { return m_expressions.end(); }
ILFunction::ExpressionList::const_iterator ILFunction::begin() const { return m_expressions.begin(); }
ILFunction::ExpressionList::const_iterator ILFunction::end() const { return m_expressions.end(); }

bool ILFunction::generatePath(rd_address address, ILFunction* il, std::set<rd_address>& path)
{
    auto* g = il->context()->document()->getGraph(address);

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
