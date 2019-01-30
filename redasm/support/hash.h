#ifndef HASH_H
#define HASH_H

#include "../types/base_types.h"
#include "../redasm_buffer.h"

namespace REDasm {
namespace Hash {

template<typename T> u16 crc16(const T* data, u64 length)
{
    u8 x;
    u16 crc = 0xFFFF;
    const u8* bdata = reinterpret_cast<const u8*>(data);

    while(length--)
    {
        x = crc >> 8 ^ *bdata++;
        x ^= x >> 4;
        crc = (crc << 8) ^ static_cast<u16>(x << 12) ^ static_cast<u16>(x << 5) ^ static_cast<u16>(x);
    }

    return crc;
}

inline u16 crc16(const BufferRef& br) { return crc16(br.data(), br.size()); }
inline u16 crc16(const BufferRef& br, u64 length) { return crc16(br.data(), std::min(br.size(), length)); }
inline u16 crc16(const std::string& s) { return crc16(s.data(), s.size()); }
template<typename T> inline u16 crc16(const std::string& s) { return crc16(s.data(), s.size()); }
template<typename T, typename A> u16 crc16(const std::vector<T, A>& v) { return crc16(v.data(), v.size()); }

} // namespace Hash
} // namespace REDasm

#endif // HASH_H
