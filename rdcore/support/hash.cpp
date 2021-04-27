#include "hash.h"
#include "../libs/miniz/miniz.h"

u32 Hash::adler32(const u8* data, size_t size) { return mz_adler32(MZ_ADLER32_INIT, data, size); }

u16 Hash::crc16(const u8* data, size_t size)
{
    u8 x;
    u16 crc = 0xFFFF;

    while(size--)
    {
        x = crc >> 8 ^ *data++;
        x ^= x >> 4;
        crc = (crc << 8) ^ static_cast<u16>(x << 12) ^ static_cast<u16>(x << 5) ^ static_cast<u16>(x);
    }

    return crc;
}

u32 Hash::crc32(const u8* data, size_t size) { return mz_crc32(MZ_CRC32_INIT, data, size); }

u16 Hash::crc16(const u8* data, size_t datasize, rd_offset offset, size_t size)
{
    if(size == RD_NVAL) size = datasize;
    if((offset + size) > datasize) return 0;

    return Hash::crc16(data + offset, size);
}

u32 Hash::crc32(const u8* data, size_t datasize, rd_offset offset, size_t size)
{
    if(size == RD_NVAL) size = datasize;
    if((offset + size) > datasize) return 0;
    return Hash::crc32(data + offset, size);
}
