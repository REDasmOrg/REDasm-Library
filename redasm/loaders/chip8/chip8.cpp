#include "chip8.h"

namespace REDasm {

CHIP8Loader::CHIP8Loader(AbstractBuffer *buffer): LoaderPluginB(buffer) { }
std::string CHIP8Loader::name() const { return "CHIP-8 ROM"; }
std::string CHIP8Loader::assembler() const { return "chip8"; }
u32 CHIP8Loader::bits() const { return 8; }

void CHIP8Loader::load()
{
    m_document->segment("MEMORY", 0, 0x200, 0x1000, SegmentTypes::Code | SegmentTypes::Data);
    m_document->entry(0x200);
}

} // namespace REDasm
