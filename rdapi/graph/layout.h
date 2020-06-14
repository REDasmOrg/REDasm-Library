#pragma once

#include "graph.h"

enum RDLayeredLayoutType {
    LayeredLayoutType_Medium, LayeredLayoutType_Narrow, LayeredLayoutType_Wide
};

RD_API_EXPORT bool RDGraphLayout_Layered(RDGraph* graph, rd_type type);
