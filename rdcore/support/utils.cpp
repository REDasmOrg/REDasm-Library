#include "utils.h"
#include <algorithm>
#include <codecvt>
#include <cstring>
#include <regex>
#include "../context.h"
#include "../buffer/view.h"
#include "../support/hash.h"
#include "../support/endian.h"
#include "../document/document.h"
#include "../plugin/loader.h"

#define WILDCARD_BYTE "??"

const char Utils::HEX_DIGITS[513] = "000102030405060708090A0B0C0D0E0F"
                                    "101112131415161718191A1B1C1D1E1F"
                                    "202122232425262728292A2B2C2D2E2F"
                                    "303132333435363738393A3B3C3D3E3F"
                                    "404142434445464748494A4B4C4D4E4F"
                                    "505152535455565758595A5B5C5D5E5F"
                                    "606162636465666768696A6B6C6D6E6F"
                                    "707172737475767778797A7B7C7D7E7F"
                                    "808182838485868788898A8B8C8D8E8F"
                                    "909192939495969798999A9B9C9D9E9F"
                                    "A0A1A2A3A4A5A6A7A8A9AAABACADAEAF"
                                    "B0B1B2B3B4B5B6B7B8B9BABBBCBDBEBF"
                                    "C0C1C2C3C4C5C6C7C8C9CACBCCCDCECF"
                                    "D0D1D2D3D4D5D6D7D8D9DADBDCDDDEDF"
                                    "E0E1E2E3E4E5E6E7E8E9EAEBECEDEEEF"
                                    "F0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF";

bool Utils::isCode(const SafeDocument& doc, rd_address address)
{
    RDSegment segment;
    if(!doc->addressToSegment(address, &segment)) return false;
    if(!HAS_FLAG(&segment, SegmentFlags_Code)) return false;
    return true;
}

bool Utils::isPureCode(const RDSegment* s)
{
    return HAS_FLAG(s, SegmentFlags_Code) && !HAS_FLAG(s, SegmentFlags_Data) && !HAS_FLAG(s, SegmentFlags_Bss);
}

bool Utils::toByte(const std::string& s, u8* val, u64 offset)
{
    if(offset > (s.size() - 2)) return false;
    if(!std::isxdigit(s[offset]) || !std::isxdigit(s[offset + 1])) return false;

    *val = static_cast<u8>(std::stoi(s.substr(offset, 2), nullptr, 16));
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

rd_offset Utils::findPattern(const u8* data, size_t datasize, std::string pattern, size_t* patternlen)
{
    if(!data || !datasize) return RD_NVAL;

    size_t len = 0;
    if(!Utils::checkPattern(pattern, len) || (len > datasize)) return RD_NVAL;
    if(patternlen) *patternlen = len;

    for(size_t i = 0; datasize >= len; i++, data++, datasize--)
    {
        if(!datasize) break;
        if(Utils::matchPattern(data, datasize, pattern)) return i;
    }

    return RD_NVAL;
}

std::string Utils::thunk(const std::string& s, int level)
{
    if(level > 1) return "thunk_" + std::to_string(level) + "_" + s;
    return "thunk_" + s;
}

std::string Utils::hexStringEndian(const Context* ctx, const RDBufferView* view, size_t size)
{
    if(size == RD_NVAL) size = view->size;

    if(ctx->document()->endianness() != Endianness_Invalid)
    {
        auto bits = size * CHAR_BIT;

        switch(size)
        {
            case 1: return Utils::hex(view->data[0], bits);

            case 2:
                if(ctx->document()->endianness() == Endianness_Big) return Utils::hex(Endian::frombigendian16(*reinterpret_cast<u16*>(view->data)), bits);
                else return Utils::hex(Endian::fromlittleendian16(*reinterpret_cast<u16*>(view->data)), bits);

            case 4:
                if(ctx->document()->endianness() == Endianness_Big) return Utils::hex(Endian::frombigendian32(*reinterpret_cast<u32*>(view->data)), bits);
                else return Utils::hex(Endian::fromlittleendian32(*reinterpret_cast<u32*>(view->data)), bits);

            case 8:
                if(ctx->document()->endianness() == Endianness_Big) return Utils::hex(Endian::frombigendian64(*reinterpret_cast<u64*>(view->data)), bits);
                else return Utils::hex(Endian::fromlittleendian64(*reinterpret_cast<u64*>(view->data)), bits);

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

size_t& Utils::hashCombine(size_t& s, size_t v)
{
    std::hash<size_t> h;
    s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
    return s;
}

u64 Utils::uleb128Decode(const u8* uleb128, size_t* c)
{
    u64 result = 0, shift = 0;
    size_t i = 0;
    u8 b = 0;

    do
    {
        b = *(uleb128 + i);
        result |= (b & 0x7f) << shift;
        shift += 7;
        i++;
    }
    while(b & 0x80);

    if(c) *c = i;
    return result;
}

s64 Utils::leb128Decode(const u8* leb128, size_t size, size_t* c)
{
    size *= CHAR_BIT;

    u64 result = 0, shift = 0;
    size_t i = 0;
    u8 b = 0;

    do
    {
        b = *(leb128 + i);
        result |= (b & 0x7f) << shift;
        shift += 7;
        i++;
    }
    while(b & 0x80);

    if((shift < size) && (b & 0x40)) // Sign Extend
        result |= (~0u << shift);

    if(c) *c = i;
    return static_cast<s64>(result);
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

bool Utils::matchPattern(const u8* data, size_t datasize, const std::string& pattern)
{
    const u8* pcurr = data;

    for(size_t i = 0; (i <= (pattern.size() - 2)); i += 2, pcurr++, datasize--)
    {
        if(!datasize) return false;

        const std::string& hexb = pattern.substr(i, 2);
        if(hexb == WILDCARD_BYTE) continue;

        u8 b = 0;
        if(!Utils::toByte(hexb, &b) || (b != *pcurr)) return false;
    }

    return true;
}

bool Utils::checkPattern(std::string& p, size_t& len)
{
    // Cache processed patterns
    static std::unordered_map<std::string, std::pair<std::string, size_t>> PROCESSED;

    auto it = PROCESSED.find(p);

    if(it != PROCESSED.end())
    {
        p = it->second.first;
        len = it->second.second;
        return true;
    }

    std::string op = p; // Store unprocessed pattern
    p.erase(std::remove_if(p.begin(), p.end(), ::isspace), p.end());
    if(p.empty() || (p.size() % 2)) return false;

    std::string_view v(p);
    size_t wccount = 0;
    len = 0;

    for(size_t i = 0; i < v.size() - 2; i += 2, len++)
    {
        const auto& hexb = v.substr(i, 2);

        if(hexb == WILDCARD_BYTE)
        {
            wccount++;
            continue;
        }

        if(!std::isxdigit(hexb.front()) || !std::isxdigit(hexb.back()))
            return false;
    }

   if(wccount >= p.size()) return false;
   PROCESSED[op] = {p, len}; // Cache processed pattern
   return true;
}
