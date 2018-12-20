#ifndef N64_H
#define N64_H

#include "../../plugins/plugins.h"

#define N64_ROM_HEADER_SIZE    4096
#define N64_MEDIA_FORMAT_SIZE  4
#define N64_IMAGE_NAME_SIZE    20
#define N64_CART_ID_SIZE       2
#define N64_BOOT_CODE_SIZE     4032


namespace REDasm {

struct N64RomHeader // From: http://en64.shoutwiki.com/wiki/ROM#Cartridge_ROM_Header
{
    u8 pi_bsb_dom1_lat_reg;
    u8 pi_bsb_dom1_pgs_reg;
    u8 pi_bsd_dom1_pwd_reg;
    u8 pi_bsb_dom1_pgs_reg2;
    u32 clock_rate_override;
    u32 program_counter;
    u32 release_address;
    u32 crc1;
    u32 crc2;
    u64 unknown1;                           // UNKNOWN/NOT USED
    char image_name[N64_IMAGE_NAME_SIZE];
    u32 unknown2;                           // UNKNOWN/NOT USED
    char media_format[N64_MEDIA_FORMAT_SIZE];
    char cart_id[N64_CART_ID_SIZE];
    char country_code;
    u8 version;
    char boot_code[N64_BOOT_CODE_SIZE];
};

class N64RomFormat: public FormatPluginT<N64RomHeader>
{
    public:
        N64RomFormat(Buffer& buffer);
        virtual const char* name() const;
        virtual u32 bits() const;
        virtual const char* assembler() const;
        virtual endianness_t endianness() const;
        virtual Analyzer* createAnalyzer(DisassemblerAPI *disassembler, const SignatureFiles &signatures) const;
        virtual bool load();

    private:
        u32 getEP() const;
        u8 calculateChecksum();
        u8 checkMediaType();
        u8 checkCountryCode();
        bool validateRom();
};

DECLARE_FORMAT_PLUGIN(N64RomFormat, n64rom)

}

#endif // N64_H
