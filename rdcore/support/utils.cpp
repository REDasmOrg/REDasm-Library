#include "utils.h"
#include <algorithm>
#include <codecvt>
#include <cstring>
#include <regex>
#include "../context.h"
#include "../buffer/view.h"
#include "../support/hash.h"
#include "../document/document.h"
#include "../plugin/loader.h"

bool Utils::isCode(const SafeDocument& doc, rd_address address)
{
    RDSegment segment;
    if(!doc->segment(address, &segment)) return false;
    if(!HAS_FLAG(&segment, SegmentFlags_Code)) return false;
    return true;
}

int Utils::branchDirection(rd_address fromaddress, rd_address address)
{
    return static_cast<int>(static_cast<std::make_signed<decltype(address)>::type>(address) -
                            static_cast<std::make_signed<decltype(fromaddress)>::type>(fromaddress));
}

rd_offset Utils::findIn(const u8* data, size_t datasize, const u8* finddata, size_t finddatasize)
{
    if(finddatasize > datasize) return RD_NVAL;

    for(const u8* p = data; datasize; datasize--, p++)
    {
        if(datasize < finddatasize) return RD_NVAL;
        if(!std::memcmp(p, finddata, finddatasize)) return p - data;
    }

    return RD_NVAL;
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

std::string Utils::thunk(const std::string& s, int level)
{
    if(level > 1) return "thunk_" + std::to_string(level) + "_" + s;
    return "thunk_" + s;
}

std::string Utils::hexStringEndian(const Context* ctx, const RDBufferView* view, size_t size)
{
    if(size == RD_NVAL) size = view->size;

    if(ctx->loader() && (ctx->loader()->endianness() != Endianness_Invalid))
    {
        switch(size)
        {
            case 1: return std::to_string(view->data[0]); break;

            case 2:
                if(ctx->loader()->endianness() == Endianness_Big) return Utils::hex(Endian::frombigendian16(*reinterpret_cast<u16*>(view->data)));
                else return Utils::hex(Endian::fromlittleendian16(*reinterpret_cast<u16*>(view->data)));

            case 4:
                if(ctx->loader()->endianness() == Endianness_Big) return Utils::hex(Endian::frombigendian32(*reinterpret_cast<u32*>(view->data)));
                else return Utils::hex(Endian::fromlittleendian32(*reinterpret_cast<u32*>(view->data)));

            case 8:
                if(ctx->loader()->endianness() == Endianness_Big) return Utils::hex(Endian::frombigendian64(*reinterpret_cast<u64*>(view->data)));
                else return Utils::hex(Endian::fromlittleendian64(*reinterpret_cast<u64*>(view->data)));

            default: break;
        }
    }

    return Utils::hexString(view, size);
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

std::string Utils::wildcardToRegex(const std::string& wcs)
{
    std::string s = "^" + Utils::escapeRegex(wcs);
    Utils::replaceAll(s, "\\*", ".*");
    return Utils::replaceAll(s, "\\?", ".") + "$";
}

bool Utils::matchWildcard(const std::string& s, const std::string& wc) { return Utils::matchRegex(s, Utils::wildcardToRegex(wc)); }

bool Utils::matchRegex(const std::string& s, const std::string& rgx)
{
    std::regex r(rgx);
    return std::regex_search(s, r);
}

std::string& Utils::replaceAll(std::string& s, const std::string& from, const std::string& to)
{
    size_t pos = s.find(from);

    while(pos != std::string::npos)
    {
        s.replace(pos, from.size(), to);
        pos = s.find(from, pos + to.size());
    }

    return s;
}

std::string Utils::escapeRegex(const std::string& s)
{
    static const std::regex SPECIAL_CHARS{R"([-[\]{}()*+?.,\^$|#\s])"};
    return std::regex_replace(s, SPECIAL_CHARS, R"(\$&)");
}
