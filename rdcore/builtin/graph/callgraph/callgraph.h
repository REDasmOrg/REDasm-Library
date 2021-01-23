#pragma once

#include <unordered_map>
#include <tuple>
#include <queue>
#include <list>
#include "../../../graph/styledgraph.h"
#include "callgraphitem.h"

class DocumentNet;

class CallGraph: public StyledGraph
{
    private:
        typedef std::tuple<const DocumentNetNode*, CallGraphItem*> CallItem;

    public:
        CallGraph(Context* ctx);
        void walk(rd_address address);

    private:
        void cgraph(DocumentNet* net, const DocumentNetNode* nn);
        void cgraph(DocumentNet* net, CallGraphItem* cgitem, rd_address address);
        void cgraph(DocumentNet* net, const DocumentNetNode* nn, CallGraphItem* cgitem);
        std::pair<CallGraphItem*, bool> pushCallItem(const DocumentNetNode* nn);

    private:
        std::unordered_map<const DocumentNetNode*, CallGraphItem*> m_done;
        std::queue<CallItem> m_queue;
        std::list<CallGraphItem> m_items;
};

