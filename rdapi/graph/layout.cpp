#include "layout.h"
#include <rdcore/builtin/graph/layeredlayout.h>

bool RDGraphLayout_Layered(RDGraph* graph, rd_type type)
{
    LayeredLayout ll(CPTR(StyledGraph, graph), type);
    return ll.execute();
}
