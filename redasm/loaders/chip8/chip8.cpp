#include "chip8.h"

namespace REDasm {

LOADER_PLUGIN_TEST(CHIP8Loader, u8)
{
    if(REDasm::pathext_is(request.filepath, "chip8"))
        return true;

    if(REDasm::pathext_is(request.filepath, "ch8"))
        return true;

    if(REDasm::pathext_is(request.filepath, "rom"))
        return true;

    return false;
}

CHIP8Loader::CHIP8Loader(AbstractBuffer *buffer): LoaderPluginB(buffer) { }
std::string CHIP8Loader::assembler() const { return "chip8"; }
u32 CHIP8Loader::bits() const { return 8; }

void CHIP8Loader::load()
{
    m_document->segment("MEMORY", 0, 0x200, 0x1000, SegmentTypes::Code | SegmentTypes::Data);
    m_document->entry(0x200);
}

} // namespace REDasm
