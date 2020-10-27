#include "functioncontainer.h"

FunctionGraph* FunctionContainer::findGraph(rd_address address) const
{
    auto it = std::find_if(m_graphs.begin(), m_graphs.end(), [address](const auto& item) {
        return item.second->contains(address);
    });

    return it != m_graphs.end() ? it->second.get() : nullptr;
}

void FunctionContainer::graph(FunctionGraph* g) { m_graphs[g->startAddress()] = FunctionGraphPtr(g); }
bool FunctionContainer::remove(rd_address address) { m_graphs.erase(address); return ContainerType::remove(address); }

RDLocation FunctionContainer::findFunction(rd_address address) const
{
    auto* g = this->findGraph(address);
    if(g) return { { g->startAddress() }, true };
    return { { 0 }, false };
}

const FunctionBasicBlock* FunctionContainer::findBasicBlock(rd_address address) const
{
    auto* g = this->findGraph(address);
    return g ? g->basicBlock(address) : nullptr;
}

void FunctionContainer::clearGraphs() { m_graphs.clear(); }
