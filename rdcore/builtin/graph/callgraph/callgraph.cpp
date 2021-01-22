#include "callgraph.h"
#include "../../../document/document.h"
#include "../../../rdil/ilfunction.h"
#include "../../../context.h"

CallGraph::CallGraph(Context* ctx): StyledGraph(ctx) { }

void CallGraph::walk(rd_address address)
{
    this->clear();
    m_items.clear();
    m_done.clear();

    auto& doc = this->context()->document();
    auto loc = doc->functionStart(address);
    if(loc.valid) address = loc.address;
    this->cgraph(address, RD_NVAL);
}

void CallGraph::cgraph(rd_address address, RDGraphNode parentnode)
{
    auto it = m_done.find(address);
    RDGraphNode n{ };

    if(it == m_done.end()) // Avoid infinite call recursion
    {
        ILFunction il(this->context());
        if(!ILFunction::generate(address, &il)) return;

        n = this->pushNode();
        auto& cgitem = m_items.emplace_back(n, address);
        this->setData(n, std::addressof(cgitem));
        m_done[address] = n;

        for(const ILExpression* e : il)
        {
            if(!IS_TYPE(e, RDIL_Call)) continue;
            cgitem.addCall(e);
            this->cgraph(e->u->u_value, n);
        }
    }
    else // Just recover the node and add the edge
        n = it->second;

    if(parentnode != RD_NVAL) this->pushEdge(parentnode, n);
    else this->setRoot(n);
}
