#include "binary.h"

namespace REDasm {

LOADER_PLUGIN_TEST(BinaryLoader, u8)
{
    RE_UNUSED(header);
    RE_UNUSED(view);
    return true;
}

BinaryLoader::BinaryLoader(AbstractBuffer *buffer): LoaderPluginB(buffer), m_bits(0) {  }
std::string BinaryLoader::assembler() const { return m_assembler.c_str(); }
u32 BinaryLoader::bits() const { return m_bits; }
bool BinaryLoader::isBinary() const { return true; }
void BinaryLoader::load() { }

void BinaryLoader::build(const std::string &assembler, u32 bits, offset_t offset, address_t baseaddress, address_t entrypoint, u32 segmenttype)
{
    m_assembler = assembler;
    m_bits = bits;

    m_document->segment("seg000", offset, baseaddress, m_buffer->size(), segmenttype);
    m_document->entry(entrypoint);
}

} // namespace REDasm
