#pragma once

#include <redasm/graph/edge.h>
#include "../types/containers/templates/list_template.h"

namespace REDasm {

class EdgeListImpl: public ListTemplate<Edge> { };

} // namespace REDasm
