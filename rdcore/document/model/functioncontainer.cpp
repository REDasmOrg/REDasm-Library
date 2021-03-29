#include "functioncontainer.h"

FunctionGraph* FunctionContainer::getGraph(rd_address address) const
{
    // Cache LRU graph
    if(m_lastgraph && m_lastgraph->contains(address)) return m_lastgraph;

    auto it = std::find_if(m_values.begin(), m_values.end(), [address](const auto& item) {
        return item.second->contains(address);
    });

    if(it == m_values.end()) return nullptr;
    m_lastgraph = it->second.get();
    return m_lastgraph;
}

RDLocation FunctionContainer::getFunction(rd_address address) const
{
    auto* g = this->getGraph(address);
    if(g) return { { g->startAddress() }, true };
    return { { 0 }, false };
}

bool FunctionContainer::isBasicBlockTail(rd_address address) const
{
    auto* g = this->getGraph(address);
    if(!g) return false;

    const RDGraphNode* nodes = nullptr;
    size_t c = g->nodes(&nodes);

    for(size_t i = 0; (c > 1) && (i < c - 1); i++)
    {
        const auto* fbb = reinterpret_cast<const FunctionBasicBlock*>(g->data(nodes[i])->p_data);
        if(!fbb || !fbb->contains(address)) continue;
        if(fbb->endaddress == address) return true;
    }

    return false;
}

void FunctionContainer::invalidateGraphs()
{
    m_lastgraph = nullptr;
    this->clearValues();
}
