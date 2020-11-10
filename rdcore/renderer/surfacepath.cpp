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
    const auto* items = this->context()->document()->items();
    auto firstit = items->lowerBound(*m_surface->firstItem());
    auto lastit = items->upperBound(*m_surface->lastItem());
    size_t fromrow = 0;

    for(auto it = firstit; it != lastit; it++, fromrow++)
    {
        const RDDocumentItem& item = *it;

        if(IS_TYPE(&item, DocumentItemType_Instruction))
        {
            auto* node = net->findNode(item.address);
            if(!node) continue;

            for(const auto& branch : node->branchestrue)
            {
                if(branch == item.address) continue;
                RDDocumentItem toitem{ branch, DocumentItemType_Instruction, 0};
                if(items->contains(toitem)) this->insertPath(item, toitem);
            }
        }
        else if(IS_TYPE(&item, DocumentItemType_Symbol))
        {
            RDSymbol symbol;
            if(!this->context()->document()->symbol(item.address, &symbol)) continue;
            if(!items->contains(RDDocumentItem{ item.address, DocumentItemType_Instruction, 0 })) continue;

            const rd_address* refs = nullptr;
            size_t c = net->getReferences(item.address, &refs);

            for(size_t i = 0; i < c; i++)
            {
                if(refs[i] == item.address) continue;
                RDDocumentItem fromitem{ refs[i], DocumentItemType_Instruction, 0 };
                if(items->contains(fromitem)) this->insertPath(fromitem, item);
            }
        }
    }

    std::sort(m_paths.begin(), m_paths.end(), [](const auto& p1, const auto& p2) -> bool {
        return p1.fromrow < p2.fromrow;
    });
}

void SurfacePath::insertPath(const RDDocumentItem& fromitem, const RDDocumentItem& toitem)
{
    const auto* net = this->context()->net();
    const auto* node = net->findNode(fromitem.address);
    if(!node) return;

    int fromrow = m_surface->findRow(&fromitem), torow = this->calculateToRow(&fromitem, &toitem);

    auto pair = std::make_pair(fromitem.address, toitem.address);
    auto it = m_done.insert(pair);
    if(!it.second) return;

    if(fromitem.address > toitem.address) // Loop
    {
        if(node->branchesfalse.size()) m_paths.push_back({ fromrow, torow, Theme_GraphEdgeLoopCond });
        else m_paths.push_back({ fromrow, torow, Theme_GraphEdgeLoop });
    }
    else
    {
        if(node->branchesfalse.size()) m_paths.push_back({ fromrow, torow, Theme_GraphEdgeFalse });
        else m_paths.push_back({ fromrow, torow, Theme_GraphEdge });
    }
}

int SurfacePath::calculateToRow(const RDDocumentItem* fromitem, const RDDocumentItem* toitem) const
{
    int torow = m_surface->findRow(toitem);
    if(torow != -1) return torow;

    int rows = 0;
    m_surface->getSize(&rows, nullptr);

    if(toitem->address < fromitem->address) return -1;
    else return rows + 1;
}
