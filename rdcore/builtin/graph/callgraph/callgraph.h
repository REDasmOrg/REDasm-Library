#pragma once

#include <unordered_map>
#include <list>
#include "../../../graph/styledgraph.h"
#include "callgraphitem.h"

class CallGraph: public StyledGraph
{
    public:
        CallGraph(Context* ctx);
        void walk(rd_address address);

    private:
        void cgraph(rd_address address, RDGraphNode parentnode);

    private:
        std::unordered_map<rd_address, RDGraphNode> m_done;
        std::list<CallGraphItem> m_items;
};

