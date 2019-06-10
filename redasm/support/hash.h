#pragma once

#include <vector>
#include <string>
#include "../buffer/bufferview.h"

namespace REDasm {

class Hash
{
    public: // crc16
        static u16 crc16(const BufferView* br);
        static u16 crc16(const BufferView* br, size_t length);
        static u16 crc16(const std::string& s);
        static u16 crc16(const u8* data, size_t length);
        template<typename T, typename A> static inline u16 crc16(const std::vector<T, A>& v) { return crc16(v.data(), v.size()); }

    public: // crc32
        static u32 crc32(const BufferView* br);
        static u32 crc32(const BufferView* br, size_t length);
        static u32 crc32(const std::string& s);
        static u32 crc32(const u8* data, size_t length);
        template<typename T, typename A> static inline u32 crc32(const std::vector<T, A>& v) { return crc32(v.data(), v.size()); }
};

} // namespace REDasm
