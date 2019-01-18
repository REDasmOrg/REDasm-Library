#include "binary.h"

namespace REDasm {

FORMAT_PLUGIN_TEST(BinaryFormat, u8)
{
    RE_UNUSED(format);
    RE_UNUSED(buffer);
    return true;
}

BinaryFormat::BinaryFormat(Buffer &buffer): FormatPluginB(buffer), m_bits(0) {  }
const char *BinaryFormat::name() const { return "Binary Format"; }
const char *BinaryFormat::assembler() const { return m_assembler.c_str(); }
u32 BinaryFormat::bits() const { return m_bits; }
u32 BinaryFormat::flags() const { return FormatFlags::Binary; }
void BinaryFormat::load() { }

void BinaryFormat::build(const std::string &assembler, u32 bits, offset_t offset, address_t baseaddress, address_t entrypoint, u32 segmenttype)
{
    m_assembler = assembler;
    m_bits = bits;

    m_document->segment("seg000", offset, baseaddress, m_buffer.size(), segmenttype);
    m_document->entry(entrypoint);
}

} // namespace REDasm
