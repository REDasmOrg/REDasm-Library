#include "utils.h"
#include <algorithm>
#include <codecvt>
#include <cstring>
#include "../buffer/view.h"

const u8* Utils::findBytes(const u8* bytes, size_t bytesize, const u8* data, size_t datasize)
{
    for(const u8* p = bytes; bytesize; bytesize--, p++)
    {
        if(bytesize < datasize) return nullptr;
        if(!std::memcmp(p, data, datasize)) return p;
    }

    return nullptr;
}

std::string Utils::hexString(BufferView* view)
{
    std::stringstream ss;

    for(size_t i = 0; i < view->size(); i++)
    {
        ss << std::uppercase << std::setfill('0') <<
              std::setw(2) << std::hex <<
              static_cast<size_t>(view->at(i));
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
