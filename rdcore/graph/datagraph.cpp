#include "datagraph.h"

const RDGraphData* DataGraph::data(RDGraphNode n) const
{
    auto it = m_data.find(n);
    return (it != m_data.end()) ? std::addressof(it->second) : nullptr;
}

void DataGraph::setData(RDGraphNode n, uintptr_t val) { m_data[n].nu_data = val; }
void DataGraph::setData(RDGraphNode n, intptr_t val) { m_data[n].ns_data = val; }
void DataGraph::setData(RDGraphNode n, const char* val) { m_data[n].s_data = val; }
void DataGraph::setData(RDGraphNode n, void* val) { m_data[n].p_data = val; }
