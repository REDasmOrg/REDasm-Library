#include "hash_impl.h"
#include <libs/miniz/miniz.h>

#undef crc32

namespace REDasm {

u16 REDasm::HashImpl::crc16(const u8 *data, size_t length)
{
    u16 crc = 0xFFFF;

    while(length--) {
        u8 x = crc >> 8 ^ *data++;
        x ^= x >> 4;
        crc = (crc << 8) ^ static_cast<u16>(x << 12) ^ static_cast<u16>(x << 5) ^ static_cast<u16>(x);
    }

    return crc;
}

u32 HashImpl::crc32(const u8 *data, size_t length) { return mz_crc32(0, data, length); }

} // namespace REDasm
