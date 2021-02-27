#include "functioncontainer.h"

FunctionGraph* FunctionContainer::findGraph(rd_address address) const
{
    auto it = std::find_if(m_values.begin(), m_values.end(), [address](const auto& item) {
        return item.second->contains(address);
    });

    return it != m_values.end() ? it->second.get() : nullptr;
}

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

