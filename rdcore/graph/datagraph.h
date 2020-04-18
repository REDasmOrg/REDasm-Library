#pragma once

#include <unordered_map>
#include "graph.h"

class DataGraph: public Graph
{
    public:
        DataGraph() = default;
        const RDGraphData* data(RDGraphNode n) const;
        void setData(RDGraphNode n, uintptr_t val);
        void setData(RDGraphNode n, intptr_t val);
        void setData(RDGraphNode n, const char* val);
        void setData(RDGraphNode n, void* val);

    private:
        std::unordered_map<RDGraphNode, RDGraphData> m_data;
};

