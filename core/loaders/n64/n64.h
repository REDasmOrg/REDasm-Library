#pragma once

#include "../../plugins/plugins.h"

#define N64_ROM_HEADER_SIZE    4096
#define N64_MEDIA_FORMAT_SIZE  4
#define N64_IMAGE_NAME_SIZE    20
#define N64_CART_ID_SIZE       2
#define N64_BOOT_CODE_SIZE     4032

#define N64_ROM_CHECKSUM_START      0x00001000
#define N64_ROM_CHECKSUM_LENGTH     0x00100000
#define N64_ROM_CHECKSUM_CIC_6102   0xF8CA4DDC      // From n64crc (http://n64dev.org/n64crc.html)
#define N64_ROM_CHECKSUM_CIC_6103   0xA3886759      // From n64crc (http://n64dev.org/n64crc.html)
#define N64_ROM_CHECKSUM_CIC_6105   0xDF26F436      // From n64crc (http://n64dev.org/n64crc.html)
#define N64_ROM_CHECKSUM_CIC_6106   0x1FEA617A      // From n64crc (http://n64dev.org/n64crc.html)

#define N64_BOOT_CODE_CIC_6101_CRC       0x6170A4A1     // CIC 6101 BOOT CODE CRC
#define N64_BOOT_CODE_CIC_7102_CRC       0x009E9EA3     // CIC 7102 BOOT CODE CRC
#define N64_BOOT_CODE_CIC_6102_CRC       0x90BB6CB5     // CIC 6102-7101 BOOT CODE CRC
#define N64_BOOT_CODE_CIC_6103_CRC       0x0B050EE0     // CIC 6103-7103 BOOT CODE CRC
#define N64_BOOT_CODE_CIC_6105_CRC       0x98BC2C86     // CIC 6105-7105 BOOT CODE CRC
#define N64_BOOT_CODE_CIC_6106_CRC       0xACC8580A     // CIC 6106-7106 BOOT CODE CRC


namespace REDasm {

struct N64RomHeader // From: http://en64.shoutwiki.com/wiki/ROM#Cartridge_ROM_Header
{
    union {
        u32be magic;

        struct {
            union { u8 magic_0, pi_bsb_dom1_lat_reg;  };
            union { u8 magic_1, pi_bsb_dom1_pgs_reg;  };
            union { u8 magic_2, pi_bsd_dom1_pwd_reg;  };
            union { u8 magic_3, pi_bsb_dom1_pgs_reg2; };
        };
    };

    u32be clock_rate_override, program_counter, release_address;
    u32be crc1, crc2;
    u64be unknown1; // UNKNOWN/NOT USED
    char image_name[N64_IMAGE_NAME_SIZE];
    u32be unknown2; // UNKNOWN/NOT USED
    char media_format[N64_MEDIA_FORMAT_SIZE];
    char cart_id[N64_CART_ID_SIZE];
    char country_code;
    u8 version;
    char boot_code[N64_BOOT_CODE_SIZE];
};

class N64Loader: public LoaderPluginT<N64RomHeader>
{
    PLUGIN_NAME("Nintendo 64 ROM")
    DECLARE_LOADER_PLUGIN_TEST(N64RomHeader)

    public:
        N64Loader(AbstractBuffer* buffer);
        std::string assembler() const override;
        Analyzer* createAnalyzer(DisassemblerAPI *disassembler) const override;
        void load() override;

    public:
        static bool checkMediaType(const N64RomHeader* header);
        static bool checkCountryCode(const N64RomHeader* header);
        static bool checkChecksum(const N64RomHeader *header, const BufferView &view);

    private:
        static bool getBootcodeAndSeed(const N64RomHeader* header, u32* bootcode, u32* seed);
        static u32 calculateChecksum(const N64RomHeader *header, const BufferView &view, u32 *crc);
        static u32 getCICVersion(const N64RomHeader *header);
        u32 getEP();
};

DECLARE_LOADER_PLUGIN(N64Loader, n64rom)

}
