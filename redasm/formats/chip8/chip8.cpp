#include "chip8.h"

namespace REDasm {

CHIP8Format::CHIP8Format(AbstractBuffer *buffer): FormatPluginB(buffer) { }
std::string CHIP8Format::name() const { return "CHIP-8 ROM"; }
std::string CHIP8Format::assembler() const { return "chip8"; }
u32 CHIP8Format::bits() const { return 8; }

void CHIP8Format::load()
{
    m_document->segment("MEMORY", 0, 0x200, 0x1000, SegmentTypes::Code | SegmentTypes::Data);
    m_document->entry(0x200);
}

} // namespace REDasm
