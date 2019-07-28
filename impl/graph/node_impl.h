#pragma once

#include <redasm/graph/node.h>
#include "../types/containers/templates/list_template.h"

namespace REDasm {

class NodeListImpl: public ListTemplate<Node> { };

} // namespace REDasm
