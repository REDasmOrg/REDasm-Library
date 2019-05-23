#include "loader.h"

namespace REDasm {

LOADER_PLUGIN_TEST(LoaderPluginB, u8)
{
    RE_UNUSED(request);
    RE_UNUSED(header);
    return true;
}

LoaderPlugin::LoaderPlugin(AbstractBuffer* buffer): Plugin()
{
    m_buffer = std::unique_ptr<AbstractBuffer>(buffer); // Take ownership
    m_view = m_buffer->view(0);                         // Full View
}

ListingDocument& LoaderPlugin::createDocument() { m_document = ListingDocument(); return m_document; }
const ListingDocument &LoaderPlugin::document() const { return m_document; }
ListingDocument& LoaderPlugin::document() { return m_document; }
SignatureIdentifiers &LoaderPlugin::signatures() { return m_signatures; }

offset_location LoaderPlugin::offset(address_t address) const
{
    for(const Segment& segment : m_document->segments())
    {
        if(!segment.contains(address))
            continue;

        offset_t offset = (address - segment.address) + segment.offset;
        return REDasm::make_location(offset, segment.containsOffset(offset));
    }

    return REDasm::invalid_location<offset_t>();
}

address_location LoaderPlugin::address(offset_t offset) const
{
    for(const Segment& segment : m_document->segments())
    {
        if(!segment.containsOffset(offset))
            continue;

        address_t address = (offset - segment.offset) + segment.address;
        return REDasm::make_location(address, segment.contains(address));
    }

    return REDasm::invalid_location<address_t>();
}

void LoaderPlugin::build(const std::string &assembler, offset_t offset, address_t baseaddress, address_t entrypoint) { throw std::runtime_error("Invalid call to LoaderPlugin::build()"); }
Analyzer* LoaderPlugin::createAnalyzer(DisassemblerAPI *disassembler) const { return new Analyzer(disassembler); }
std::string LoaderPlugin::assembler() const { return std::string(); }
AbstractBuffer *LoaderPlugin::buffer() const { return m_buffer.get(); }
BufferView LoaderPlugin::viewOffset(offset_t offset) const { return m_buffer->view(offset); }

BufferView LoaderPlugin::view(address_t address) const
{
    offset_location offset = this->offset(address);

    if(!offset.valid)
        return BufferView();

    return this->viewOffset(offset);
}

BufferView LoaderPlugin::viewSegment(const Segment *segment) const
{
    if(!segment || segment->empty() || segment->is(SegmentType::Bss))
        return BufferView();

    return m_buffer->view(segment->offset, segment->rawSize());
}

} // namespace REDasm
