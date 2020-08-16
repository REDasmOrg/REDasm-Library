#include "rendereritem.h"
#include <algorithm>

void RendererItem::setDocumentIndex(size_t index) { m_docindex = index; }
const std::string& RendererItem::text() const { return m_text; }
size_t RendererItem::size() const { return m_formats.size(); }
std::string RendererItem::formatText(const RDRendererFormat* format) const { return m_text.substr(format->start, (format->end - format->start) + 1); }

RendererItem& RendererItem::push(const std::string& text, rd_type fgtheme, rd_type bgtheme)
{
    size_t start = m_text.size();

    RDRendererFormat f{ };
    f.start = static_cast<s32>(start);
    f.end = static_cast<s32>(start + text.size() - 1);
    f.fgtheme = fgtheme;
    f.bgtheme = bgtheme;

    m_formats.push_back(f);
    m_text += text;
    return *this;
}

RendererItem::FormatsIterator RendererItem::unformat(s32 start, s32 end)
{
    size_t begidx = this->indexFromPos(start);
    size_t endidx = this->indexFromPos(end);

    RDRendererFormat begrf = m_formats[begidx], endrf = m_formats[endidx];

    auto it = m_formats.erase(std::next(m_formats.begin(), begidx),
                              std::next(m_formats.begin(), endidx + 1));

    begrf.end = start - 1; // Shrink first part
    endrf.start = end + 1; // Shrink last part

    if(!RendererItem::formatEmpty(&begrf))
    {
        it = m_formats.insert(it, begrf);
        it++;
    }

    if(!RendererItem::formatEmpty(&endrf))
        it = m_formats.insert(it, endrf);

    return it;
}

const RDRendererFormat& RendererItem::format(size_t idx) const { return m_formats[idx]; }

RendererItem& RendererItem::format(s32 start, s32 end, rd_type fgtheme, rd_type bgtheme)
{
    if(m_text.empty() || (start >= m_text.size()))
        return *this;

    end = std::min<s32>(end, m_text.size() - 1);

    RDRendererFormat f{ };
    f.start = start;
    f.end = end;
    f.fgtheme = fgtheme;
    f.bgtheme = bgtheme;

    auto it = this->unformat(start, end);
    m_formats.insert(it, f);
    return *this;
}

size_t RendererItem::documentIndex() const { return m_docindex; }

size_t RendererItem::formats(const RDRendererFormat** formats) const
{
    if(!formats) return 0;
    *formats = m_formats.data();
    return m_formats.size();
}

size_t RendererItem::indexFromPos(s32 pos) const
{
    for(size_t i = 0; i < m_formats.size(); i++)
    {
        if(RendererItem::formatContains(&m_formats[i], pos))
            return i;
    }

    return RD_NPOS;
}

bool RendererItem::formatContains(const RDRendererFormat* rf, s32 pos) { return (pos >= rf->start) && (pos <= rf->end); }
bool RendererItem::formatEmpty(const RDRendererFormat* rf) { return rf->start > rf->end; }
