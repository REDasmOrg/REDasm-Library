#pragma once

#include <unordered_map>
#include "graph.h"

class DataGraph: public Graph
{
    public:
        DataGraph(Context* ctx);
        void clear() override;
        const RDGraphData* data(RDGraphNode n) const;
        void setData(RDGraphNode n, uintptr_t val);
        void setData(RDGraphNode n, intptr_t val);
        void setData(RDGraphNode n, const char* val);
        void setData(RDGraphNode n, void* val);
        template<typename T> RDGraphNode pushDataNode(T val);

    private:
        std::unordered_map<RDGraphNode, RDGraphData> m_data;
};

template<typename T>
RDGraphNode DataGraph::pushDataNode(T val) {
    auto n = this->pushNode();
    this->setData(n, val);
    return n;
}

