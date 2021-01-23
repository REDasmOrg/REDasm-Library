#pragma once

#include <rdapi/types.h>
#include <rdapi/graph/callgraph.h>
#include "../../../document/documentnet.h"

class CallGraphItem
{
    public:
        CallGraphItem(RDGraphNode n, const DocumentNetNode* nn);
        RDGraphNode node() const;
        const DocumentNetNode* netNode() const;

    private:
        RDGraphNode m_node;
        const DocumentNetNode* m_netnode;
};

