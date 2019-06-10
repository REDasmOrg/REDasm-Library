#pragma once

#include <redasm/types/base_types.h>

namespace REDasm {

class HashImpl
{
    public:
        HashImpl() = delete;
        static u16 crc16(const u8* data, size_t length);
        static u32 crc32(const u8* data, size_t length);
};

} // namespace REDasm
