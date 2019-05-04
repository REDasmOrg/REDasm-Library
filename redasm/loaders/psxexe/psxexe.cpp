#include "psxexe.h"
#include "psxexe_analyzer.h"
#include <cstring>

#define PSXEXE_SIGNATURE   "PS-X EXE"
#define PSXEXE_TEXT_OFFSET 0x00000800
#define PSX_USER_RAM_START 0x80000000
#define PSX_USER_RAM_END   0x80200000

namespace REDasm {

LOADER_PLUGIN_TEST(PsxExeLoader, PsxExeHeader) { return !strncmp(header->id, PSXEXE_SIGNATURE, PSXEXE_SIGNATURE_SIZE); }

PsxExeLoader::PsxExeLoader(AbstractBuffer *buffer): LoaderPluginT<PsxExeHeader>(buffer) { }
std::string PsxExeLoader::assembler() const { return "mips32le"; }
Analyzer *PsxExeLoader::createAnalyzer(DisassemblerAPI *disassembler) const { return new PsxExeAnalyzer(disassembler); }

void PsxExeLoader::load()
{
    m_signatures.insert("psyq");

    if(m_header->t_addr > PSX_USER_RAM_START)
        m_document->segment("RAM0", 0, PSX_USER_RAM_START, (m_header->t_addr - PSX_USER_RAM_START), SegmentType::Bss);

    m_document->segment("TEXT", PSXEXE_TEXT_OFFSET, m_header->t_addr, m_header->t_size, SegmentType::Code | SegmentType::Data);

    if((m_header->t_addr + m_header->t_size) < PSX_USER_RAM_END)
        m_document->segment("RAM1", 0, m_header->t_addr + m_header->t_size, PSX_USER_RAM_END - (m_header->t_addr + m_header->t_size), SegmentType::Bss);

    m_document->entry(m_header->pc0);
}

}
