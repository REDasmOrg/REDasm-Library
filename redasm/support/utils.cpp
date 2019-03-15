#include "utils.h"
#include <algorithm>
#include <sstream>
#include <cctype>

namespace REDasm {

std::string wtoa(const std::wstring& ws)
{
    std::string s;
    std::transform(ws.begin(), ws.end(), std::back_inserter(s), [](wchar_t ch) -> char {
        return static_cast<char>(ch);
    });

    return s;
}

std::string hexstring(const char *data, size_t size)
{
    std::stringstream ss;

    for(size_t i = 0; i < size; i++, data++)
    {
        ss << std::uppercase <<
              std::setfill('0') <<
              std::setw(2) <<
              std::hex <<
              static_cast<size_t>(*reinterpret_cast<const u8*>(data));
    }

    return ss.str();
}

bool byte(const std::string& s, u8 *val, u64 offset)
{
    if(offset > (s.size() - 2))
        return false;

    if(!std::isxdigit(s[offset]) || !std::isxdigit(s[offset + 1]))
        return false;

    *val = static_cast<u8>(std::stoi(s.substr(offset, 2), nullptr, 16));
    return true;
}

MemoryBuffer bytes(const std::string &s, u64 offset, u64 hexlen)
{
    if(offset >= s.length())
        return false;

    if(!hexlen || (hexlen > s.size()))
        hexlen = s.size();

    MemoryBuffer buffer;
    buffer.resize(hexlen / 2);

    for(u64 i = 0, j = 0; i < hexlen; i += 2, j++)
    {
        if(!REDasm::byte(s.substr(offset + i, 2), &buffer[j]))
            return MemoryBuffer();
    }

    return buffer;
}

std::string simplified(std::string s)
{
    std::replace_if(s.begin(), s.end(), [](char ch) -> bool {
        return std::isspace(ch);
    }, ' ');

    return s;
}

std::string pathext(const std::string &s)
{
    size_t lastidx = s.find_last_of(".");

    if(lastidx == std::string::npos)
        return std::string();

    lastidx++; // Skip '.'

    if(lastidx == std::string::npos)
        return std::string();

    return s.substr(lastidx);
}

}
