#include "rdilgraph.h"
#include "../../document/document.h"
#include "../../disassembler.h"
#include "../../context.h"
#include "../../rdil/rdil.h"
#include <queue>

RDILGraph::RDILGraph(Context* ctx): StyledGraph(ctx) { }

void RDILGraph::build(rd_address address)
{
    const auto* net = this->context()->net();
    auto* node = net->findNode(address);
    if(!node) return;

    RDBlock block;

    const auto* blocks = this->context()->document()->blocks(address);
    if(!blocks->find(address, &block)) return;

    auto* loader = this->context()->loader();
    RDBufferView view;
    if(!loader->view(address, BlockContainer::size(&block), &view)) return;

    auto* assembler = this->context()->assembler();

    ILFunction il(this->context());
    assembler->lift(address, &view, &il);
    if(!il.empty()) this->generate(&il);
}

void RDILGraph::generate(const ILFunction* il)
{
    m_strings.clear();
    this->setRoot(this->generate(il->first(), RD_NPOS));
};

RDGraphNode RDILGraph::generate(const ILExpression* e, RDGraphNode parentnode)
{
    RDGraphNode n = this->pushNode();
    this->setData(n, RDIL::getOpName(e->type));
    if(parentnode != RD_NPOS) this->pushEdge(parentnode, n);

    if(RDIL::hasValue(e))
    {
        RDGraphNode l = this->pushNode();
        this->pushEdge(n, l);

        switch(e->type)
        {
            case RDIL_Cnst:
                m_strings.push_back(Utils::hex(e->u_value));
                this->setData(l, m_strings.back().c_str());
                break;

            case RDIL_Var: this->setData(l, e->var); break;
            case RDIL_Reg: this->setData(l, e->reg); break;
            default: this->setData(l, "???"); break;
        }
    }
    else
    {
        if(e->n1) this->pushEdge(n, this->generate(e->n1, n));
        if(e->n2) this->pushEdge(n, this->generate(e->n2, n));
        if(e->n3) this->pushEdge(n, this->generate(e->n3, n));
    }

    return n;
}
