#include "format.h"

namespace REDasm {

FormatPlugin::FormatPlugin(AbstractBuffer* buffer): Plugin()
{
    m_buffer = std::unique_ptr<AbstractBuffer>(buffer); // Take ownership
    m_view = m_buffer->view(0);                         // Full View
    m_document->m_format = this;
}

ListingDocument &FormatPlugin::document() { return m_document; }
const SignatureFiles &FormatPlugin::signatures() const { return m_signatures; }
u64 FormatPlugin::addressWidth() const { return this->bits() / 8; }

offset_t FormatPlugin::offset(address_t address) const
{
    for(size_t i = 0; i < m_document->segmentsCount(); i++)
    {
        const Segment* segment = m_document->segmentAt(i);

        if(segment->contains(address))
            return (address - segment->address) + segment->offset;
    }

    return address;
}

address_t FormatPlugin::address(offset_t offset) const
{
    for(size_t i = 0; i < m_document->segmentsCount(); i++)
    {
        const Segment* segment = m_document->segmentAt(i);

        if(segment->containsOffset(offset))
            return (offset - segment->offset) + segment->address;
    }

    return offset;
}

Analyzer* FormatPlugin::createAnalyzer(DisassemblerAPI *disassembler, const SignatureFiles& signatures) const { return new Analyzer(disassembler, signatures); }
bool FormatPlugin::isBinary() const { return false; }
AbstractBuffer *FormatPlugin::buffer() const { return m_buffer.get(); }
BufferView FormatPlugin::viewOffset(offset_t offset) const { return m_buffer->view(offset); }
BufferView FormatPlugin::view(address_t address) const { return this->viewOffset(this->offset(address)); }

BufferView FormatPlugin::viewSegment(const Segment *segment) const
{
    if(!segment || segment->empty() || segment->is(SegmentTypes::Bss))
        return BufferView();

    return m_buffer->view(segment->offset, segment->size());
}

}
