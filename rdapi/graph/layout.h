#pragma once

#include "graph.h"

enum RDLayeredLayoutType {
    LayeredLayoutType_Medium, LayeredLayoutType_Narrow, LayeredLayoutType_Wide
};

RD_API_EXTERN_C bool RDGraphLayout_Layered(RDGraph* graph, type_t type);
