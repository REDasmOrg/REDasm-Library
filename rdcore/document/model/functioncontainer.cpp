#include "functioncontainer.h"

FunctionGraph* FunctionContainer::getGraph(rd_address address) const
{
    auto it = m_values.lower_bound(address);
    if(it == m_values.end()) return nullptr;

    // Search near this location (backwards)
    while(!it->second->contains(address))
    {
        if(it == m_values.begin())
        {
            if(!it->second->contains(address)) return nullptr;
            break;
        }

        it--;
    }

    return it->second.get();
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

void FunctionContainer::invalidateGraphs() { this->clearValues(); }
