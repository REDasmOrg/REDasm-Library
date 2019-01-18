#include <zlib.h>
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

#define N64_MAGIC_BS            0x37804012
#define N64_MAGIC_BE            0x80371240
#define N64_MAGIC_LE            0x40123780
#define N64_MAGIC_BE_B1         0x80
#define N64_MAGIC_BS_B1         0x37
#define N64_MAGIC_LE_B1         0x40

namespace REDasm {

FORMAT_PLUGIN_TEST(N64RomFormat, N64RomHeader)
{
    u32 magic = Endianness::cfbe<u32>(buffer);

    if((magic != N64_MAGIC_BS) && (magic != N64_MAGIC_BE) && (magic != N64_MAGIC_LE))
        return false;

    Buffer swbuffer;

    if(magic == N64_MAGIC_BS)
    {
        swbuffer = buffer.swapEndianness<u16>(sizeof(N64RomHeader)); // Swap the header
        format = static_cast<const N64RomHeader*>(swbuffer);
    }

    if(!N64RomFormat::checkMediaType(format) || !N64RomFormat::checkCountryCode(format))
        return false;

    if(!swbuffer.empty()) // Swap all
    {
        swbuffer = buffer.swapEndianness<u16>();
        format = static_cast<const N64RomHeader*>(swbuffer);
    }

    return N64RomFormat::checkChecksum(format, swbuffer.empty() ? buffer : swbuffer);
}

N64RomFormat::N64RomFormat(Buffer &buffer): FormatPluginT<N64RomHeader>(buffer) { }
const char *N64RomFormat::name() const { return "Nintendo 64 ROM"; }
u32 N64RomFormat::bits() const { return 64; }

const char *N64RomFormat::assembler() const
{
    if(m_format->magic[0] == N64_MAGIC_LE_B1)
        "mips64le";

    return "mips64be";
}

endianness_t N64RomFormat::endianness() const
{
    if(m_format->magic[0] == N64_MAGIC_LE_B1)
        return Endianness::LittleEndian;

    return Endianness::BigEndian;
}

Analyzer *N64RomFormat::createAnalyzer(DisassemblerAPI *disassembler, const SignatureFiles &signatures) const { return new N64Analyzer(disassembler, signatures); }

void N64RomFormat::load()
{
    if(m_format->magic[0] == N64_MAGIC_BS_B1)
        m_buffer.swapEndianness<u16>();

    m_document->segment("KSEG0", N64_ROM_HEADER_SIZE, this->getEP(), m_buffer.size()-N64_ROM_HEADER_SIZE, SegmentTypes::Code | SegmentTypes::Data);
    // TODO: map other segments
    m_document->entry(this->getEP());
}

u32 N64RomFormat::getEP()
{
    u32 pc = Endianness::cfbe(static_cast<u32>(m_format->program_counter));
    u32 cic_version = N64RomFormat::getCICVersion(m_format);

    if(cic_version != 0)
    {
        if(cic_version == 6103)         // CIC 6103 EP manipulation
            pc -= 0x100000;
        else if (cic_version == 6106)   // CIC 6106 EP manipulation
            pc -= 0x200000;
    }

    return pc;
}

u32 N64RomFormat::calculateChecksum(const N64RomHeader* format, const Buffer& buffer, u32 *crc) // Adapted from n64crc (http://n64dev.org/n64crc.html)
{
    u32 bootcode, i;
    u32 seed;

    u32 t1, t2, t3;
    u32 t4, t5, t6;
    u32 r, d;

    switch((bootcode = N64RomFormat::getCICVersion(format)))
    {
        case 6101:
        case 7102:
        case 6102:
            seed = N64_ROM_CHECKSUM_CIC_6102;
            break;
        case 6103:
            seed = N64_ROM_CHECKSUM_CIC_6103;
            break;
        case 6105:
            seed = N64_ROM_CHECKSUM_CIC_6105;
            break;
        case 6106:
            seed = N64_ROM_CHECKSUM_CIC_6106;
            break;
        default:
            return 1;
    }

    t1 = t2 = t3 = t4 = t5 = t6 = seed;

    i = N64_ROM_CHECKSUM_START;
    while (i < (N64_ROM_CHECKSUM_START + N64_ROM_CHECKSUM_LENGTH))
    {
        d = Endianness::cfbe(*reinterpret_cast<const u32*>(&buffer[i]));

        if ((t6 + d) < t6) t4++;
        t6 += d;
        t3 ^= d;
        r = REDasm::rol(d, (d & 0x1F));
        t5 += r;
        if (t2 > d) t2 ^= r;
        else t2 ^= t6 ^ d;

        if (bootcode == 6105) t1 += Endianness::cfbe(*reinterpret_cast<const u32*>(&buffer[N64_ROM_HEADER_SIZE + 0x0710 + (i & 0xFF)])) ^ d;
        else t1 += t5 ^ d;

        i += 4;
    }

    if(bootcode == 6103)
    {
        crc[0] = (t6 ^ t4) + t3;
        crc[1] = (t5 ^ t2) + t1;
    }
    else if(bootcode == 6106)
    {
        crc[0] = (t6 * t4) + t3;
        crc[1] = (t5 * t2) + t1;
    }
    else
    {
        crc[0] = t6 ^ t4 ^ t3;
        crc[1] = t5 ^ t2 ^ t1;
    }

    return 0;
}

bool N64RomFormat::checkChecksum(const N64RomHeader *format, const Buffer& buffer)
{
    u32 crc[2] = { 0 };

    if(!N64RomFormat::calculateChecksum(format, buffer, crc))
    {
        if((crc[0] == Endianness::cfbe(format->crc1)) && (crc[1] == Endianness::cfbe(format->crc2)))
            return true;
    }

    return false;
}

u32 N64RomFormat::getCICVersion(const N64RomHeader* format)
{
    u64 boot_code_crc = crc32(0L, reinterpret_cast<const u8*>(format->boot_code), N64_BOOT_CODE_SIZE);
    u32 cic = 0;

    switch(boot_code_crc)
    {
        case N64_BOOT_CODE_CIC_6101_CRC:
            cic = 6101;
            break;

        case N64_BOOT_CODE_CIC_7102_CRC:
            cic = 7102;
            break;

        case N64_BOOT_CODE_CIC_6102_CRC:
            cic = 6102;
            break;

        case N64_BOOT_CODE_CIC_6103_CRC:
            cic = 6103;
            break;

        case N64_BOOT_CODE_CIC_6105_CRC:
            cic = 6105;
            break;

        case N64_BOOT_CODE_CIC_6106_CRC:
            cic = 6106;
            break;

        default:
            break;
    }

    return cic;
}

bool N64RomFormat::checkMediaType(const N64RomHeader *format)
{
    switch(format->media_format[3])
    {
        case 'N': // Cart
        case 'D': // 64DD disk
        case 'C': // Cartridge part of expandable game
        case 'E': // 64DD expansion for cart
        case 'Z': // Aleck64 cart
            return true;

        default:
            break;
    }

    return false;
}

bool N64RomFormat::checkCountryCode(const N64RomHeader *format)
{
    /*
     * 0x37 '7' "Beta"
     * 0x41 'A' "Asian (NTSC)"
     * 0x42 'B' "Brazilian"
     * 0x43 'C' "Chinese"
     * 0x44 'D' "German"
     * 0x45 'E' "North America"
     * 0x46 'F' "French"
     * 0x47 'G': Gateway 64 (NTSC)
     * 0x48 'H' "Dutch"
     * 0x49 'I' "Italian"
     * 0x4A 'J' "Japanese"
     * 0x4B 'K' "Korean"
     * 0x4C 'L': Gateway 64 (PAL)
     * 0x4E 'N' "Canadian"
     * 0x50 'P' "European (basic spec.)"
     * 0x53 'S' "Spanish"
     * 0x55 'U' "Australian"
     * 0x57 'W' "Scandinavian"
     * 0x58 'X' "European"
     * 0x59 'Y' "European"
     */

    if(format->country_code == 0x37)
        return true;
    else if((format->country_code >= 0x41) && (format->country_code <= 0x4C))
        return true;
    else if((format->country_code == 0x4E) || (format->country_code == 0x50))
        return true;
    else if((format->country_code == 0x53) || (format->country_code == 0x55))
        return true;
    else if((format->country_code >= 0x57) && (format->country_code <= 0x59))
        return true;

    return false;
}

}
