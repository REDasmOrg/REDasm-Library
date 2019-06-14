#pragma once

#include <redasm/types/base_types.h>
#include <string>

namespace REDasm {

class HashImpl
{
    public:
        HashImpl() = delete;
        static u16 crc16(const u8* data, size_t length);
        static u32 crc32(const u8* data, size_t length);
        static std::string base64encode(const u8* data, size_t length);
        static std::string base64decode(const char *b64string);

    private:
        static inline bool isBase64(u8 c);

    private:
        static const std::string BASE64_CHARS;
};

} // namespace REDasm
