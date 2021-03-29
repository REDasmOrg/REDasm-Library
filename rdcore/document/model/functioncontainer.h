#pragma once

#include <rdapi/types.h>
#include <memory>
#include "../../builtin/graph/functiongraph/functiongraph.h"
#include "../../containers/addresscontainer.h"

class FunctionGraph;

typedef std::unique_ptr<FunctionGraph> FunctionGraphPtr;

class FunctionContainer: public AddressContainer<FunctionGraphPtr>
{
    public:
        FunctionContainer() = default;
        FunctionGraph* getGraph(rd_address address) const;
        RDLocation getFunction(rd_address address) const;
        bool isBasicBlockTail(rd_address address) const;
        void invalidateGraphs();

    private:
        mutable FunctionGraph* m_lastgraph{nullptr};
};

