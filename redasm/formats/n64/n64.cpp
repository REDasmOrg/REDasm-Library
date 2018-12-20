#include "n64.h"
#include "n64_analyzer.h"


// https://level42.ca/projects/ultra64/Documentation/man/pro-man/pro09/index9.3.html
// http://en64.shoutwiki.com/wiki/ROM#Cartridge_ROM_Header

#define N64_KUSEG_START_ADDR   0x00000000   // TLB mapped
#define N64_KUSEG_SIZE         0x7fffffff

#define N64_KSEG0_START_ADDR   0x80000000   // Direct mapped, cached
#define N64_KSEG0_SIZE         0x1FFFFFFF

#define N64_KSEG1_START_ADDR   0xa0000000   // Direct mapped, uncached
#define N64_KSEG1_SIZE         0x1FFFFFFF

#define N64_KSSEG_START_ADDR   0xc0000000   // TLB mapped
#define N64_KSSEG_SIZE         0x1FFFFFFF

#define N64_KSEG3_START_ADDR   0xe0000000   // TLB mapped
#define N64_KSEG3_SIZE         0x1FFFFFFF

#define N64_SEGMENT_AREA(name) N64_##name##_START_ADDR, N64_##name##_SIZE

namespace REDasm {

N64RomFormat::N64RomFormat(Buffer &buffer): FormatPluginT<N64RomHeader>(buffer) { }
const char *N64RomFormat::name() const { return "Nintendo 64 ROM"; }
u32 N64RomFormat::bits() const { return 64; }
const char *N64RomFormat::assembler() const { return "mips32be"; }

endianness_t N64RomFormat::endianness() const
{
    return Endianness::BigEndian;
}

Analyzer *N64RomFormat::createAnalyzer(DisassemblerAPI *disassembler, const SignatureFiles &signatures) const
{
    return new N64Analyzer(disassembler, signatures);
}

bool N64RomFormat::load()
{
    if(!this->validateRom())
        return false;

    m_document.segment("KSEG0", N64_ROM_HEADER_SIZE, Endianness::cfbe(static_cast<u32>(m_format->program_counter)), m_buffer.size()-N64_ROM_HEADER_SIZE, SegmentTypes::Code | SegmentTypes::Data);
    // TODO: map other segments
    m_document.entry(this->getEP());
    return true;
}

u32 N64RomFormat::getEP() const
{
    u32 pc = Endianness::cfbe(static_cast<u32>(m_format->program_counter)); // TODO: Check CIC entry point redirection
    return pc;
}

u8 N64RomFormat::calculateChecksum()
{
    // FIXME implement CRC check
}

u8 N64RomFormat::checkMediaType()
{
    bool result = false;

    switch (m_format->media_format[3]) {
    case 'N':           // Cart
        result = true;
        break;
    case 'D':           // 64DD disk
        result = true;
        break;
    case 'C':           // Cartridge part of expandable game
        result = true;
        break;
    case 'E':           // 64DD expansion for cart
        result = true;
        break;
    case 'Z':           // Aleck64 cart
        result = true;
        break;
    default:
        result = false;
        break;
    }

    return result;
}

u8 N64RomFormat::checkCountryCode()
{
    /*
     0x37 '7' "Beta"
     0x41 'A' "Asian (NTSC)"
     0x42 'B' "Brazilian"
     0x43 'C' "Chinese"
     0x44 'D' "German"
     0x45 'E' "North America"
     0x46 'F' "French"
     0x47 'G': Gateway 64 (NTSC)
     0x48 'H' "Dutch"
     0x49 'I' "Italian"
     0x4A 'J' "Japanese"
     0x4B 'K' "Korean"
     0x4C 'L': Gateway 64 (PAL)
     0x4E 'N' "Canadian"
     0x50 'P' "European (basic spec.)"
     0x53 'S' "Spanish"
     0x55 'U' "Australian"
     0x57 'W' "Scandinavian"
     0x58 'X' "European"
     0x59 'Y' "European"
    */

    if(m_format->country_code == 0x37)
        return true;
    else if(m_format->country_code >= 0x41 &&  m_format->country_code <= 0x4C)
        return true;
    else if(m_format->country_code == 0x4E || m_format->country_code == 0x50)
        return true;
    else if(m_format->country_code == 0x53 || m_format->country_code == 0x55)
        return true;
    else if(m_format->country_code >= 0x57 &&  m_format->country_code <= 0x59)
        return true;
    else
        return false;
}

bool N64RomFormat::validateRom()
{
    u32 magic_number = static_cast<u32>(m_buffer);

    if(magic_number != 0x80371240 && magic_number != 0x37804012)
        return false;

    if(m_buffer.size() < N64_ROM_HEADER_SIZE)
        return false;

    if(m_format->pi_bsb_dom1_lat_reg == 0x37)
        return false;   // TODO: FIXME add LE to BE conversion

    if(!N64RomFormat::checkMediaType())
        return false;

    if(!N64RomFormat::checkCountryCode())
        return false;

    return true;
}

}
