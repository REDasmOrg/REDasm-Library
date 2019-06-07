#pragma once

#include <vector>
#include <string>
#include "../buffer/bufferview.h"

namespace REDasm {

class Hash
{
    public:
        static inline u16 crc16(const BufferView* br) { return crc16(br->data(), br->size()); }
        static inline u16 crc16(const BufferView* br, u64 length) { return crc16(br->data(), std::min(br->size(), length)); }
        static inline u16 crc16(const std::string& s) { return crc16(s.data(), s.size()); }
        template<typename T> static inline u16 crc16(const std::string& s) { return crc16(s.data(), s.size()); }
        template<typename T, typename A> static inline u16 crc16(const std::vector<T, A>& v) { return crc16(v.data(), v.size()); }
        template<typename T> static u16 crc16(const T* data, u64 length);
};

template<typename T> u16 Hash::crc16(const T *data, u64 length)
{
    u16 crc = 0xFFFF;
    const u8* bdata = reinterpret_cast<const u8*>(data);

    while(length--) {
        u8 x = crc >> 8 ^ *bdata++;
        x ^= x >> 4;
        crc = (crc << 8) ^ static_cast<u16>(x << 12) ^ static_cast<u16>(x << 5) ^ static_cast<u16>(x);
    }

    return crc;
}

} // namespace REDasm
