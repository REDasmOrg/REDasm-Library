#pragma once

#include <type_traits>
#include <functional>
#include <climits>
#include <sstream>
#include <iomanip>
#include <string>
#include <deque>
#include <rdapi/types.h>
#include "../support/error.h"
#include "../document/document_fwd.h"

struct RDBufferView;
class Context;

class Utils
{
    private:
        typedef std::deque<std::string> StringContainer;

    public:
        Utils() = delete;
        static bool isCode(const SafeDocument& doc, rd_address address);
        static bool toByte(const std::string& s, u8 *val, u64 offset = 0);
        static int branchDirection(rd_address fromaddress, rd_address address);
        static inline u8* relpointer(void* ptr, size_t offset) { return reinterpret_cast<u8*>(reinterpret_cast<u8*>(ptr) + offset); }
        static rd_offset findIn(const u8* data, size_t datasize, const u8* finddata, size_t finddatasize);
        static rd_offset findPattern(const u8* data, size_t datasize, std::string pattern, size_t* patternlen = nullptr);
        static u16 crc16(const u8* data, size_t datasize, rd_offset offset = 0, size_t size = RD_NVAL);
        static u32 crc32(const u8* data, size_t datasize, rd_offset offset = 0, size_t size = RD_NVAL);
        static std::string thunk(const std::string& s, int level = 1);
        static std::string hexStringEndian(const Context* ctx, const RDBufferView* view, size_t size = RD_NVAL);
        static std::string hexString(const RDBufferView* view, size_t size = RD_NVAL);
        static std::string toString(const std::u16string& s);
        static std::string quoted(const std::u16string& s);
        static std::string quoted(const std::string& s);
        static std::string quotedSingle(const std::u16string& s);
        static std::string quotedSingle(const std::string& s);
        static std::string simplified(const std::u16string& s);
        static std::string simplified(std::string s);
        static StringContainer split(const std::string& s, char sep);
        static std::string wildcardToRegex(const std::string& wcs);
        static bool matchWildcard(const std::string& s, const std::string& wc);
        static bool matchRegex(const std::string& s, const std::string& rgx);
        static size_t& hashCombine(size_t& s, size_t v);

    public:
        template<typename Container> static std::string join(const Container& c, const char* sep);
        template<typename T> static std::string number(T value, size_t base = 10, size_t width = 0, char fill = '\0');
        template<typename T> static std::string hex(T t, size_t bits = 0, bool withprefix = false);
        template<typename T> static typename std::make_signed<T>::type signext(T val, int valbits);
        template<typename T> static T rol(T val, T amt);
        template<typename T> static T ror(T val, T amt);

    private:
        static std::string& replaceAll(std::string& s, const std::string& from, const std::string& to);
        static std::string escapeRegex(const std::string& s);
        static bool matchPattern(const u8* data, size_t datasize, const std::string& pattern);
        static bool checkPattern(std::string& p, size_t& len);
};

template<typename T>
T Utils::rol(T val, T amt)
{
    static const T BITS_COUNT = sizeof(T) * CHAR_BIT;
    if(!amt) return val;
    return (val << amt) | (val >> (BITS_COUNT - amt));
}

template<typename T>
T Utils::ror(T val, T amt)
{
    static const T BITS_COUNT = sizeof(T) * CHAR_BIT;
    if(!amt) return val;
    return (val >> amt) | (val << (BITS_COUNT - amt));
}

template<typename T>
typename std::make_signed<T>::type Utils::signext(T val, int valbits)
{
    T m = 1;
    m <<= valbits - 1;
    return static_cast<typename std::make_signed<T>::type>((val ^ m) - m);
}

template<typename Container>
std::string Utils::join(const Container& c, const char* sep)
{
    std::stringstream ss;

    for(auto it = c.begin(); it != c.end(); it++) {
        if(sep && (it != c.begin())) ss << sep;
        ss << *it;
    }

    return ss.str();
}

template<typename T>
std::string Utils::number(T value, size_t base, size_t width, char fill)
{
    if((base == 10) && !width && !fill) return std::to_string(value);

    std::stringstream ss;

    if(base == 8) ss << std::oct;
    else if(base == 16) ss << std::hex;

    if(width) ss << std::setw(width);
    if(fill) ss << std::setfill(fill);

    if(sizeof(T) == 1)
    {
        if(std::is_signed<T>::value) ss << static_cast<typename std::make_signed<size_t>::type>(value);
        else ss << static_cast<size_t>(value);
    }
    else
        ss << value;

    return ss.str();
}

template<typename T>
std::string Utils::hex(T value, size_t bits, bool withprefix)
{
    std::stringstream ss;
    if(withprefix && (value > (std::is_signed<T>::value ? 9 : 9u))) ss << "0x";

    ss << std::uppercase << std::hex;

    if(bits > 0) ss << std::setfill('0') << std::setw(bits / 4);

    if(std::is_signed<T>::value && value < 0) ss << "-" << (~value) + 1;
    else ss << value;

    return ss.str();
}
