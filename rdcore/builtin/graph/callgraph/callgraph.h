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
        typedef std::tuple<const DocumentNetNode*, CallGraphItem*, int> CallItem;

    public:
        CallGraph(Context* ctx);
        void walk(rd_address address);
        CallGraphItem* walkFrom(rd_address address);

    private:
        CallGraphItem* cgraph(DocumentNet* net, const DocumentNetNode* nn, int maxdepth = 1);
        void cgraph(DocumentNet* net, CallGraphItem* cgitem, rd_address address, int depth);
        void cgraph(DocumentNet* net, const DocumentNetNode* nn, CallGraphItem* cgitem, int depth);
        std::pair<CallGraphItem*, bool> pushCallItem(const DocumentNetNode* nn);

    private:
        int m_maxdepth;
        std::unordered_map<const DocumentNetNode*, CallGraphItem*> m_done;
        std::queue<CallItem> m_queue;
        std::list<CallGraphItem> m_items;
};

