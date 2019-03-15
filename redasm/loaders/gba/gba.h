#ifndef GBALOADER_H
#define GBALOADER_H

#include "../../plugins/plugins.h"

#define GBA_ROM_HEADER_SIZE    192
#define GBA_NINTENDO_LOGO_SIZE 156
#define GBA_GAME_TITLE_SIZE    12
#define GBA_GAME_CODE_SIZE     4
#define GBA_MAKER_CODE_SIZE    2

namespace REDasm {

struct GbaRomHeader // From: http://problemkaputt.de/gbatek.htm#gbacartridgeheader
{
    u32 entry_point;
    u8 nintendo_logo[GBA_NINTENDO_LOGO_SIZE];
    char game_title[GBA_GAME_TITLE_SIZE];
    char game_code[GBA_GAME_CODE_SIZE];
    char maker_code[GBA_MAKER_CODE_SIZE];
    u8 fixed_val;
    u8 main_unit_code;
    u8 device_type;
    u8 reserved_area[7];
    u8 software_version;
    u8 header_checksum;
    u8 reserved_area_2[2];
    u32 ram_entry_point;
    u8 boot_mode;
    u8 slave_id;
    u8 unused[26];
    u32 joybus_entry_point;
};

class GbaLoader: public LoaderPluginT<GbaRomHeader>
{
    PLUGIN_NAME("Game Boy Advance ROM")
    DECLARE_LOADER_PLUGIN_TEST(GbaRomHeader)

    public:
        GbaLoader(AbstractBuffer* buffer);
        virtual std::string assembler() const;
        virtual u32 bits() const;
        virtual void load();

    public:
        static bool isUppercaseAscii(const char* s, size_t c);
        static u8 calculateChecksum(const BufferView& view);

    private:
        u32 getEP() const;
};

DECLARE_LOADER_PLUGIN(GbaLoader, gbarom)

}

#endif // GBALOADER_H
