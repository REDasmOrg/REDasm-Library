#include "format.h"

namespace REDasm {

FormatPlugin::FormatPlugin(AbstractBuffer* buffer): Plugin()
{
    m_buffer = std::unique_ptr<AbstractBuffer>(buffer); // Take ownership
    m_view = m_buffer->view(0);                         // Full View
}

ListingDocument& FormatPlugin::createDocument() { m_document = ListingDocument(); return m_document; }
ListingDocument &FormatPlugin::document() { return m_document; }
const SignatureFiles &FormatPlugin::signatures() const { return m_signatures; }
u64 FormatPlugin::addressWidth() const { return this->bits() / 8; }

offset_location FormatPlugin::offset(address_t address) const
{
    for(size_t i = 0; i < m_document->segmentsCount(); i++)
    {
        const Segment* segment = m_document->segmentAt(i);

        if(segment->contains(address))
        {
            offset_t offset = (address - segment->address) + segment->offset;
            return REDasm::make_location(offset, segment->containsOffset(offset));
        }
    }

    return REDasm::invalid_location<offset_t>();
}

address_location FormatPlugin::address(offset_t offset) const
{
    for(size_t i = 0; i < m_document->segmentsCount(); i++)
    {
        const Segment* segment = m_document->segmentAt(i);

        if(segment->containsOffset(offset))
        {
            address_t address = (offset - segment->offset) + segment->address;
            return REDasm::make_location(address, segment->contains(address));
        }
    }

    return REDasm::invalid_location<address_t>();
}

Analyzer* FormatPlugin::createAnalyzer(DisassemblerAPI *disassembler, const SignatureFiles& signatures) const { return new Analyzer(disassembler, signatures); }
bool FormatPlugin::isBinary() const { return false; }
AbstractBuffer *FormatPlugin::buffer() const { return m_buffer.get(); }
BufferView FormatPlugin::viewOffset(offset_t offset) const { return m_buffer->view(offset); }

BufferView FormatPlugin::view(address_t address) const
{
    offset_location offset = this->offset(address);

    if(!offset.valid)
        return BufferView();

    return this->viewOffset(offset);
}

BufferView FormatPlugin::viewSegment(const Segment *segment) const
{
    if(!segment || segment->empty() || segment->is(SegmentTypes::Bss))
        return BufferView();

    return m_buffer->view(segment->offset, segment->size());
}

}
