#include "abstractlayout.h"

namespace REDasm {
namespace Graphing {

AbstractLayout::AbstractLayout(Graph *graph): m_graph(graph) { }
int AbstractLayout::middleX(const Node &n) const { return m_graph->x(n) + (m_graph->width(n) / 2); }
int AbstractLayout::middleY(const Node &n) const { return m_graph->y(n) + (m_graph->height(n) / 2);  }
int AbstractLayout::bottom(const Node &n) const  { return m_graph->y(n) + m_graph->height(n); }
int AbstractLayout::right(const Node &n) const   { return m_graph->x(n) + m_graph->width(n); }

} // namespace Graphing
} // namespace REDasm
