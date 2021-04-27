#pragma once

#include <type_traits>
#include <functional>
#include <climits>
#include <sstream>
#include <iomanip>
#include <limits>
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
        static bool isPureCode(const RDSegment* s);
        static bool toByte(const std::string& s, u8 *val, u64 offset = 0);
        static int branchDirection(rd_address fromaddress, rd_address address);
        static inline u8* relpointer(void* ptr, size_t offset) { return reinterpret_cast<u8*>(reinterpret_cast<u8*>(ptr) + offset); }
        static rd_offset findIn(const u8* data, size_t datasize, const u8* finddata, size_t finddatasize);
        static rd_offset findPattern(const u8* data, size_t datasize, std::string pattern, size_t* patternlen = nullptr);
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
        static u64 uleb128Decode(const u8* uleb128, size_t* c);
        static s64 leb128Decode(const u8* leb128, size_t size, size_t* c);

    public:
        template<typename Container> static std::string join(const Container& c, const char* sep);
        template<typename T> static std::string number(T value, size_t base = 10, size_t width = 0, char fill = '\0');
        template<typename T> static std::string hex(T value, size_t bits = 0, bool withprefix = false);
        template<typename T> static typename std::make_signed<T>::type signext(T val, int valbits);
        template<typename T> static T rol(T val, T amt);
        template<typename T> static T ror(T val, T amt);

    private:
        template<typename T> static std::string hexSigned(T t, size_t bits = 0, bool withprefix = false);
        static std::string& replaceAll(std::string& s, const std::string& from, const std::string& to);
        static std::string escapeRegex(const std::string& s);
        static bool matchPattern(const u8* data, size_t datasize, const std::string& pattern);
        static bool checkPattern(std::string& p, size_t& len);

    private:
        static const char HEX_DIGITS[513];
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
    std::string s;

    for(auto it = c.begin(); it != c.end(); it++) {
        if(sep && (it != c.begin())) s += sep;
        s += *it;
    }

    return s;
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
    if constexpr(std::is_unsigned<T>::value) {
        uint64_t v = static_cast<uint64_t>(value);

        int i = static_cast<int>(bits / CHAR_BIT) - 1;

        if(i <= 0) { // Guess bits
            if(v <= std::numeric_limits<u8>::max()) i = 0;
            else if(v <= std::numeric_limits<u16>::max()) i = 1;
            else if(v <= std::numeric_limits<u32>::max()) i = 3;
            else i = 7;
        }

        std::string result(i * 2 + 2, '0');

        while(i >= 0) {
            int pos = (v & 0xFF) * 2;
            char ch = Utils::HEX_DIGITS[pos];
            result[i * 2] = ch;
            ch = Utils::HEX_DIGITS[pos + 1];
            result[i * 2 + 1] = ch;
            v >>= 8;
            i -= 1;
        }

        return withprefix ? ("0x" + result) : result;
    }

    return Utils::hexSigned(value, bits, withprefix);
}

template<typename T>
std::string Utils::hexSigned(T value, size_t bits, bool withprefix)
{
    bool negative = false;

    if(value < 0) {
        value = (~value) + 1;
        negative = true;
    }

    uint64_t v = static_cast<uint64_t>(value);
    int i = (bits / CHAR_BIT) - 1;

    if(i <= 0) { // Guess bits
        if(v <= std::numeric_limits<u8>::max()) i = 0;
        else if(v <= std::numeric_limits<u16>::max()) i = 1;
        else if(v <= std::numeric_limits<u32>::max()) i = 3;
        else i = 7;
    }

    std::string result(i * 2 + 2, '0');

    while(i >= 0) {
        int pos = (v & 0xFF) * 2;
        char ch = Utils::HEX_DIGITS[pos];
        result[i * 2] = ch;
        ch = Utils::HEX_DIGITS[pos + 1];
        result[i * 2 + 1] = ch;
        v >>= 8;
        i -= 1;
    }

    if(negative) {
        if(withprefix) return "-0x" + result;
        else return "-" + result;
    }

    return withprefix ? ("0x" + result) : result;
}
