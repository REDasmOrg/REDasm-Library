#include "utils.h"
#include <algorithm>
#include <codecvt>
#include <cstring>
#include "../buffer/view.h"
#include "../support/hash.h"

int Utils::branchDirection(rd_address fromaddress, rd_address address)
{
    return static_cast<int>(static_cast<std::make_signed<decltype(address)>::type>(address) -
                            static_cast<std::make_signed<decltype(fromaddress)>::type>(fromaddress));
}

rd_offset Utils::findIn(const u8* data, size_t datasize, const u8* finddata, size_t finddatasize)
{
    if(finddatasize > datasize) return RD_NPOS;

    for(const u8* p = data; datasize; datasize--, p++)
    {
        if(datasize < finddatasize) return RD_NPOS;
        if(!std::memcmp(p, finddata, finddatasize)) return p - data;
    }

    return RD_NPOS;
}

u16 Utils::crc16(const u8* data, size_t datasize, rd_offset offset, size_t size)
{
    if((offset + size) > datasize) return 0;
    return Hash::crc16(data + offset, size);
}

u32 Utils::crc32(const u8* data, size_t datasize, rd_offset offset, size_t size)
{
    if((offset + size) > datasize) return 0;
    return Hash::crc32(data + offset, size);
}

std::string Utils::hexString(const RDBufferView* view, size_t size)
{
    std::stringstream ss;

    for(size_t i = 0; i < std::min<size_t>(view->size, size); i++)
    {
        ss << std::uppercase << std::setfill('0') <<
              std::setw(2) << std::hex <<
              static_cast<size_t>(view->data[i]);
    }

    return ss.str();
}

std::string Utils::toString(const std::u16string& s)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> converter;
    return converter.to_bytes(s);
}

std::string Utils::quoted(const std::u16string& s) { return Utils::quoted(Utils::toString(s)); }
std::string Utils::quotedSingle(const std::u16string& s) { return Utils::quotedSingle(Utils::toString(s)); }
std::string Utils::quoted(const std::string& s) { return "\"" + s + "\""; }
std::string Utils::quotedSingle(const std::string& s) { return "\"" + s + "\""; }
std::string Utils::simplified(const std::u16string& s) { return Utils::simplified(Utils::toString(s)); }

std::string Utils::simplified(std::string s)
{
    std::string res;

    for(char ch : s)
    {
        switch(ch)
        {
            case '\t': res += "\\t"; break;
            case '\n': res += "\\n"; break;
            case '\r': res += "\\r"; break;
            default: res += ch;
        }
    }

    return res;
}

Utils::StringContainer Utils::split(const std::string& s, char sep)
{
    StringContainer parts;
    std::string part;
    std::stringstream ss(s);

    while(std::getline(ss, part, sep))
    {
        if(part.empty()) continue;
        parts.push_back(part);
    }

    return parts;
}
