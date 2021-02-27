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
        FunctionGraph* findGraph(rd_address address) const;
        RDLocation findFunction(rd_address address) const;
        const FunctionBasicBlock* findBasicBlock(rd_address address) const;
};

