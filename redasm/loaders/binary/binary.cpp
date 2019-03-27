#include "binary.h"

namespace REDasm {

LOADER_PLUGIN_TEST(BinaryLoader, u8)
{
    RE_UNUSED(request);
    RE_UNUSED(header);
    return true;
}

BinaryLoader::BinaryLoader(AbstractBuffer *buffer): LoaderPluginB(buffer) {  }
std::string BinaryLoader::assembler() const { return m_assembler; }
void BinaryLoader::load() { /* NOP */ }

void BinaryLoader::build(const std::string &assembler, offset_t offset, address_t baseaddress, address_t entrypoint)
{
    m_assembler = assembler;
    size_t vsize = m_buffer->size();

    if(entrypoint >= vsize)
        vsize = entrypoint << 1;

    m_document->segment("BINARY", offset, baseaddress, m_buffer->size(), vsize, SegmentTypes::Code | SegmentTypes::Data);
    m_document->entry(baseaddress + entrypoint);
}

} // namespace REDasm
