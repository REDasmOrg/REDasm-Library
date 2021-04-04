#include "surfacepath.h"
#include "surface.h"
#include "../context.h"
#include "../document/document.h"
#include <rdapi/config.h>
#include <algorithm>

SurfacePath::SurfacePath(Surface* sf): Object(sf->context()), m_surface(sf) { }

size_t SurfacePath::getPath(const RDPathItem** path) const
{
    if(path) *path = m_paths.data();
    return m_paths.size();
}

void SurfacePath::update()
{
    m_paths.clear();
    m_done.clear();

    const auto* net = this->context()->net();
    const auto& doc = this->context()->document();
    const auto& rows = m_surface->rows();

    for(int i = 0; i < m_surface->lastRow(); i++)
    {
        const auto& row = rows[i];

        auto* node = net->findNode(row.address);
        if(!node) continue;

        auto flags = doc->getFlags(row.address);
        if(flags & AddressFlags_Imported) continue;

        if(flags & AddressFlags_Location)
        {
            for(const auto& from : node->from)
            {
                if(from == row.address) continue;

                auto flags = doc->getFlags(from);
                if(flags & AddressFlags_Imported) continue;
                this->insertPath(from, row.address);
            }
        }
        else
        {
            for(const auto& branch : node->branchestrue)
            {
                if(branch == row.address) continue;

                auto flags = doc->getFlags(branch);
                if(flags & AddressFlags_Imported) continue;
                this->insertPath(row.address, branch);
            }
        }
    }

    std::sort(m_paths.begin(), m_paths.end(), [](const auto& p1, const auto& p2) -> bool {
        return p1.fromrow < p2.fromrow;
    });
}

void SurfacePath::insertPath(rd_address fromaddress, rd_address toaddress)
{
    const auto* net = this->context()->net();
    const auto* node = net->findNode(fromaddress);
    if(!node) return;

    int fromrow = m_surface->lastIndexOf(fromaddress), torow = this->calculateToRow(fromaddress, toaddress);

    auto pair = std::make_pair(fromaddress, toaddress);
    auto it = m_done.insert(pair);
    if(!it.second) return;

    if(fromaddress > toaddress) // Loop
    {
        if(node->branchesfalse.size()) m_paths.push_back({ fromrow, torow, Theme_GraphEdgeLoopCond });
        else m_paths.push_back({ fromrow, torow, Theme_GraphEdgeLoop });
    }
    else
    {
        if(node->branchesfalse.size()) m_paths.push_back({ fromrow, torow, Theme_Success });
        else m_paths.push_back({ fromrow, torow, Theme_GraphEdge });
    }
}

int SurfacePath::calculateToRow(rd_address fromaddress, rd_address toaddress) const
{
    int torow = m_surface->lastIndexOf(toaddress);
    if(torow != -1) return torow;

    int rows = 0;
    m_surface->getSize(&rows, nullptr);

    if(toaddress < fromaddress) return -1;
    else return rows + 1;
}
