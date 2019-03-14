#include "binary.h"

namespace REDasm {

LOADER_PLUGIN_TEST(BinaryLoader, u8)
{
    RE_UNUSED(request);
    RE_UNUSED(header);
    return true;
}

BinaryLoader::BinaryLoader(AbstractBuffer *buffer): LoaderPluginB(buffer), m_bits(0) {  }
std::string BinaryLoader::assembler() const { return m_assembler.c_str(); }
u32 BinaryLoader::bits() const { return m_bits; }
void BinaryLoader::load() { /* NOP */ }

void BinaryLoader::build(const std::string &assembler, offset_t offset, address_t baseaddress, address_t entrypoint)
{
    m_assembler = assembler;
    m_bits = REDasm::countbits_r(baseaddress);

    if(!m_bits)
        m_bits = 32; // Default is 32 bits

    size_t vsize = m_buffer->size();

    if(entrypoint >= vsize)
        vsize = entrypoint << 1;

    m_document->segment("BINARY", offset, baseaddress, m_buffer->size(), vsize, SegmentTypes::Code | SegmentTypes::Data);
    m_document->entry(baseaddress + entrypoint);
}

} // namespace REDasm
