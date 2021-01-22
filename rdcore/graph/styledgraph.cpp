#include "styledgraph.h"
#include <algorithm>

StyledGraph::StyledGraph(Context* ctx): DataGraph(ctx) { }

void StyledGraph::clear()
{
    DataGraph::clear();
    m_nodeattributes.clear();
    m_edgeattributes.clear();
    m_areawidth = m_areaheight = 0;
}

void StyledGraph::color(const RDGraphEdge* e, const std::string& c) { m_edgeattributes[*e].color = c; }
void StyledGraph::label(const RDGraphEdge* e, const std::string& l) { m_edgeattributes[*e].label = l; }

void StyledGraph::routes(const RDGraphEdge* e, const RDGraphPoint* p, size_t n)
{
    m_edgeattributes[*e].routes.resize(n);
    std::copy_n(p, n, m_edgeattributes[*e].routes.data());
}

void StyledGraph::arrow(const RDGraphEdge* e, const RDGraphPoint* p, size_t n)
{
    m_edgeattributes[*e].arrow.resize(n);
    std::copy_n(p, n, m_edgeattributes[*e].arrow.data());
}

void StyledGraph::areaWidth(int w) { m_areawidth = w; }
void StyledGraph::areaHeight(int h) { m_areaheight = h; }
void StyledGraph::x(RDGraphNode n, int px) { m_nodeattributes[n].x = px; }
void StyledGraph::y(RDGraphNode n, int py) { m_nodeattributes[n].y = py; }
void StyledGraph::width(RDGraphNode n, int w) { m_nodeattributes[n].width = w; }
void StyledGraph::height(RDGraphNode n, int h) { m_nodeattributes[n].height = h; }
int StyledGraph::areaWidth() const { return m_areawidth; }
int StyledGraph::areaHeight() const { return m_areaheight; }

const char* StyledGraph::color(const RDGraphEdge* e) const
{
    auto it = m_edgeattributes.find(*e);
    return (it != m_edgeattributes.end()) ? it->second.color.c_str() : nullptr;
}

const char* StyledGraph::label(const RDGraphEdge* e) const
{
    auto it = m_edgeattributes.find(*e);
    return (it != m_edgeattributes.end()) ? it->second.label.c_str() : nullptr;
}

size_t StyledGraph::routes(const RDGraphEdge* e, const RDGraphPoint** path) const
{
    auto it = m_edgeattributes.find(*e);
    if(it == m_edgeattributes.end()) return 0;

    if(path) *path = it->second.routes.data();
    return it->second.routes.size();
}

size_t StyledGraph::arrow(const RDGraphEdge* e, const RDGraphPoint** path) const
{
    auto it = m_edgeattributes.find(*e);
    if(it == m_edgeattributes.end()) return 0;

    if(path) *path = it->second.arrow.data();
    return it->second.arrow.size();
}

int StyledGraph::x(RDGraphNode n) const
{
    auto it = m_nodeattributes.find(n);
    return (it != m_nodeattributes.end()) ? it->second.x : 0;
}

int StyledGraph::y(RDGraphNode n) const
{
    auto it = m_nodeattributes.find(n);
    return (it != m_nodeattributes.end()) ? it->second.y : 0;
}

int StyledGraph::width(RDGraphNode n) const
{
    auto it = m_nodeattributes.find(n);
    return (it != m_nodeattributes.end()) ? it->second.width : 0;
}

int StyledGraph::height(RDGraphNode n) const
{
    auto it = m_nodeattributes.find(n);
    return (it != m_nodeattributes.end()) ? it->second.height : 0;
}
