#include "callgraphitem.h"

CallGraphItem::CallGraphItem(RDGraphNode n, const DocumentNetNode* nn): m_node(n), m_netnode(nn) {  }
RDGraphNode CallGraphItem::node() const { return m_node; }
const DocumentNetNode* CallGraphItem::netNode() const { return m_netnode; }
