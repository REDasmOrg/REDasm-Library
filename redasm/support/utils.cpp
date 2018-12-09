#include "utils.h"
#include <algorithm>
#include <sstream>

namespace REDasm {

std::string wtoa(const std::wstring& ws)
{
    std::string s;
    std::transform(ws.begin(), ws.end(), std::back_inserter(s), [](wchar_t ch) -> char {
        return static_cast<char>(ch);
    });

    return s;
}

std::string quoted(const std::string &s) { return "\"" + s + "\""; }
std::string quoted(const char* s) { return REDasm::quoted(std::string(s)); }

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

u8 byte(const std::string& s, int offset)
{
    std::stringstream ss;

    if(offset < 0)
        ss << std::hex << s.substr(s.size() + offset, 2);
    else
        ss << std::hex << s.substr(offset, 2);

    u64 val = 0;
    ss >> val;

    return static_cast<u8>(val);
}

Buffer bytes(const std::string &s)
{
    Buffer buffer;
    buffer.resize(s.size() / 2);

    for(size_t i = 0, j = 0; i < s.size(); i += 2, j++)
         buffer[j] = REDasm::byte(s, i);

    return buffer;
}

}
