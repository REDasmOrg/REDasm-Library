#include "callgraph.h"
#include "../../../document/document.h"
#include "../../../context.h"

CallGraph::CallGraph(Context* ctx): StyledGraph(ctx) { }

void CallGraph::walk(rd_address address)
{
    this->clear();
    m_items.clear();
    m_done.clear();

    auto* cgitem = this->walkFrom(address);
    if(cgitem) this->setRoot(cgitem->node());
}

CallGraphItem* CallGraph::walkFrom(rd_address address)
{
    auto& doc = this->context()->document();
    auto loc = doc->functionStart(address);
    if(loc.valid) address = loc.address;

    auto* net = doc->net();
    auto* nn = net->findNode(address);
    return nn ? this->cgraph(net, nn) : nullptr;
}

CallGraphItem* CallGraph::cgraph(DocumentNet* net, const DocumentNetNode* nn, int maxdepth)
{
    m_maxdepth = maxdepth;

    auto [cgitem, added] = this->pushCallItem(nn);
    m_queue.push({ nn, cgitem, 0 });

    while(!m_queue.empty())
    {
        auto cg = m_queue.front();
        m_queue.pop();
        this->cgraph(net, std::get<0>(cg), std::get<1>(cg), std::get<2>(cg));
    }

    return cgitem;
}

void CallGraph::cgraph(DocumentNet* net, CallGraphItem* cgitem, rd_address address, int depth)
{
    auto* nn = net->findNode(address);
    if(nn) m_queue.push({ nn, cgitem, depth});
}

void CallGraph::cgraph(DocumentNet* net, const DocumentNetNode* nn, CallGraphItem* cgitem, int depth)
{
    if(depth >= m_maxdepth) return;

    for(auto* node = nn; node; node = net->nextNode(node))
    {
        for(rd_address branch : node->branchestrue)
        {
            if(branch <= node->address) continue; // Ignore loops
            this->cgraph(net, cgitem, branch, depth);
        }

        for(rd_address branch : node->branchesfalse)
        {
            if(branch <= node->address) continue; // Ignore loops
            this->cgraph(net, cgitem, branch, depth);
        }

        for(rd_address call : node->calls)
        {
            auto* cnn = net->findNode(call);
            if(!cnn) continue;

            auto [ccgitem, added] = this->pushCallItem(cnn);
            this->pushEdge(cgitem->node(), ccgitem->node());
            if(added) m_queue.push({ cnn, ccgitem, depth + 1 });
        }
    }
}

std::pair<CallGraphItem*, bool> CallGraph::pushCallItem(const DocumentNetNode* nn)
{
    auto it = m_done.find(nn);
    if(it != m_done.end()) return { it->second, false };

    auto n = this->pushNode();
    auto& cgitem = m_items.emplace_back(n, nn);
    this->setData(n, &cgitem);

    m_done[nn] = &cgitem;
    return { &cgitem, true };
}
