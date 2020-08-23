#include "rdilgraph.h"
#include "../../disassembler.h"
#include "../../rdil/rdil.h"
#include <queue>

RDILGraph::RDILGraph(const Disassembler* disassembler): StyledGraph(), m_disassembler(disassembler) { }

void RDILGraph::build(rd_address address)
{
    const auto* net = m_disassembler->net();
    auto* node = net->findNode(address);
    if(!node) return;

    RDBlock block;

    const auto* blocks = m_disassembler->document()->blocks(address);
    if(!blocks->find(address, &block)) return;

    auto* loader = m_disassembler->loader();
    RDBufferView view;
    if(!loader->view(address, BlockContainer::size(&block), &view)) return;

    auto* assembler = m_disassembler->assembler();

    ILFunction il(m_disassembler);

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

            case RDIL_Addr:
                m_strings.push_back(Utils::hex(e->address));
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
