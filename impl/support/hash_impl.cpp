#include "hash_impl.h"
#include <redasm/static/crc32.h>
#include <cstring>
#include <cctype>

//
// Base64 Algorithm based on: https://github.com/ReneNyffenegger/cpp-base64/blob/master/base64.cpp
//

namespace REDasm {

const String HashImpl::BASE64_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                      "abcdefghijklmnopqrstuvwxyz"
                                      "0123456789+/";

u16 REDasm::HashImpl::crc16(const u8 *data, size_t length)
{
    u16 crc = 0xFFFF;

    while(length--)
    {
        u8 x = crc >> 8 ^ *data++;
        x ^= x >> 4;
        crc = (crc << 8) ^ static_cast<u16>(x << 12) ^ static_cast<u16>(x << 5) ^ static_cast<u16>(x);
    }

    return crc;
}

u32 HashImpl::crc32(const u8 *data, size_t length)
{
    u32 crc = ~0u;

    while(length--)
        crc = Detail::CRC32_TABLE[(crc ^ *data++) & 0xFF] ^ (crc >> 8);

    return crc ^ ~0U;
}

String HashImpl::base64encode(const u8 *data, size_t length)
{
    String ret;
    int i = 0, j = 0;
    u8 chararray3[3], chararray4[4];

    while(length--)
    {
        chararray3[i++] = *(data++);

        if(i == 3)
        {
            chararray4[0] = (chararray3[0]  & 0xfc) >> 2;
            chararray4[1] = ((chararray3[0] & 0x03) << 4) + ((chararray3[1] & 0xf0) >> 4);
            chararray4[2] = ((chararray3[1] & 0x0f) << 2) + ((chararray3[2] & 0xc0) >> 6);
            chararray4[3] = chararray3[2]   & 0x3f;

            for(i = 0; i < 4; i++)
                ret += HashImpl::BASE64_CHARS[chararray4[i]];

            i = 0;
        }
    }

    if (i)
    {
        for(j = i; j < 3; j++)
            chararray3[j] = '\0';

        chararray4[0] = ( chararray3[0] & 0xfc) >> 2;
        chararray4[1] = ((chararray3[0] & 0x03) << 4) + ((chararray3[1] & 0xf0) >> 4);
        chararray4[2] = ((chararray3[1] & 0x0f) << 2) + ((chararray3[2] & 0xc0) >> 6);

        for (j = 0; (j < i + 1); j++)
            ret += HashImpl::BASE64_CHARS[chararray4[j]];

        while((i++ < 3))
            ret += '=';
    }

    return ret;
}

String HashImpl::base64decode(const char* b64string)
{
    int len = std::strlen(b64string);
    int i = 0, idx = 0;
    u8 chararray4[4], chararray3[3];
    String ret;

    while(len-- && (b64string[idx] != '=') && HashImpl::isBase64(b64string[idx]))
    {
        chararray4[i++] = b64string[idx];
        idx++;

        if(i == 4)
        {
            for (i = 0; i <4; i++)
                chararray4[i] = HashImpl::BASE64_CHARS.indexOf(chararray4[i]);

            chararray3[0] = ( chararray4[0] << 2       ) + ((chararray4[1] & 0x30) >> 4);
            chararray3[1] = ((chararray4[1] & 0xf) << 4) + ((chararray4[2] & 0x3c) >> 2);
            chararray3[2] = ((chararray4[2] & 0x3) << 6) +   chararray4[3];

            for (i = 0; (i < 3); i++)
                ret += chararray3[i];

            i = 0;
        }
    }

    if (i)
    {
        for(int j = 0; j < i; j++)
            chararray4[j] = HashImpl::BASE64_CHARS.indexOf(chararray4[j]);

        chararray3[0] = (chararray4[0] << 2) + ((chararray4[1] & 0x30) >> 4);
        chararray3[1] = ((chararray4[1] & 0xf) << 4) + ((chararray4[2] & 0x3c) >> 2);

        for(int j = 0; j < i - 1; j++)
            ret += chararray3[j];
    }

    return ret;
}

bool HashImpl::isBase64(u8 c) { return (std::isalnum(c) || (c == '+') || (c == '/')); }

} // namespace REDasm
