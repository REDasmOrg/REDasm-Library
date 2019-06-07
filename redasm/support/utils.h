#pragma once

#include <algorithm>
#include <type_traits>
#include <sstream>
#include <iomanip>
#include <climits>
#include <cctype>
#include <cmath>
#include <cassert>
#include "../buffer/bufferview.h"

namespace REDasm {

template<typename T> struct bits_count { static constexpr size_t value = sizeof(T) * CHAR_BIT; };

class Utils
{
    public:
        Utils() = delete;
        static inline std::string simplified(std::string s) { std::replace_if(s.begin(), s.end(), [](char ch) -> bool { return std::isspace(ch); }, ' ');  return s; }
        static inline std::string ltrimmed(std::string s) { s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) { return !std::isspace(ch); })); return s; }
        static inline std::string rtrimmed(std::string s) { s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) { return !std::isspace(ch); }).base(), s.end()); return s; }
        static inline std::string wtoa(const std::wstring& ws) { std::string s; std::transform(ws.begin(), ws.end(), std::back_inserter(s), [](wchar_t ch) -> char { return static_cast<char>(ch); }); return s; }

    public:
        static inline bool startsWith(const std::string& s, const std::string& b) { return s.find( b) == 0; }
        static inline bool endsWith(const std::string& s, const std::string& e) { return s.size() >= e.size() && s.compare(s.size() - e.size(), e.size(), e) == 0; }
        static inline std::string trampoline(const std::string& s, const std::string& prefix = std::string()) { return prefix + "_" + s; }
        static inline std::string quoted_s(const std::string& s) {  return "'" + s + "'"; }
        static inline std::string quoted(const std::string& s) {  return "\"" + s + "\""; }
        static inline std::string quoted(const char* s) { return Utils::quoted(std::string(s)); }
        static inline std::string quoted_s(const char* s) { return Utils::quoted_s(std::string(s)); }
        static inline std::string hexstring(const BufferView* view, size_t size) { return Utils::hexstring(reinterpret_cast<const char*>(view->data()), size); }

    public:
        static inline std::string trimmed(std::string s);
        static inline std::string hexstring(const char *data, size_t size);
        static inline bool byte(const std::string& s, u8* val, size_t offset = 0);

    public:
        template<typename T, typename U> static inline T readpointer(U** p) { T v = *reinterpret_cast<T*>(*p); *p = Utils::relpointer<U>(*p, sizeof(T)); return v; }
        template<typename T, typename ST = typename signed_of<T>::type> static inline ST twoc(T val) { return static_cast<ST>((~val) + 1); }
        template<typename T, typename U> static inline T rol(T n, U c) { assert(c < bits_count<T>::value);  return !c ? n : ((n << c) | (n >> (bits_count<T>::value - c))); }
        template<typename T, typename U> static inline T ror(T n, U c) { assert(c < bits_count<T>::value); return !c ? n : ((n >> c) | (n << (bits_count<T>::value - c))); }
        template<typename T, typename U> static inline T aligned(T t, U a) { T r = t % a; return r ? (t + (a - r)) : t; }
        template<typename T> static inline size_t countbits(T val) { double bytes = std::log(static_cast<double>(val)) / std::log(256.0); return static_cast<size_t>(std::ceil(bytes)) * 8; }
        template<typename T> static inline std::string dec(T t) { std::stringstream ss; ss << t; return ss.str(); }

    public:
        template<typename T> static inline std::string quoted(T t) { return Utils::quoted(std::to_string(t)); }
        template<typename T, typename U> static inline T* relpointer(U* base, size_t offset) { return reinterpret_cast<T*>(reinterpret_cast<size_t>(base) + offset); }

    public:
        template<typename Container> static inline std::string join(const Container& c, const std::string& sep);
        template<typename T> static inline std::string hex(T t, size_t bits = 0, bool withprefix = false);
        template<typename T> static inline size_t countbits_r(T val);
};

std::string Utils::trimmed(std::string s)
{
    // Left
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));

    // Right
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());

    return s;
}

std::string Utils::hexstring(const char *data, size_t size)
{
    std::stringstream ss;

    for(size_t i = 0; i < size; i++, data++) {
        ss << std::uppercase << std::setfill('0') << std::setw(2) << std::hex <<
              static_cast<size_t>(*reinterpret_cast<const u8*>(data));
    }

    return ss.str();
}

bool Utils::byte(const std::string &s, u8 *val, size_t offset)
{
    if(offset > (s.size() - 2))
        return false;

    if(!std::isxdigit(s[offset]) || !std::isxdigit(s[offset + 1]))
        return false;

    *val = static_cast<u8>(std::stoi(s.substr(offset, 2), nullptr, 16));
    return true;
}

template<typename Container> std::string Utils::join(const Container& c, const std::string& sep)
{
    std::stringstream ss;

    for(auto it = c.begin(); it != c.end(); it++) {
        if(it != c.begin())
            ss << sep;

        ss << *it;
    }

    return ss.str();
}

template<typename T> T unmask(T val, T mask)
{
    T result = 0;

    for(T i = 0, j = 0; i < bits_count<T>::value; i++) {
        if(!(mask & (1 << i))) // Check if mask bit is set
            continue;

        if(val & (1 << i)) // Check if data bit is set
            result |= (1 << j);

        j++;
    }

    return result;
}

template<typename T> std::string wtoa(T* ws, size_t len)
{
    std::string s;
    char* p = reinterpret_cast<char*>(ws);

    for(size_t i = 0; i < len; i++, p += sizeof(char) * 2)
        s += *p;

    return s;
}

template<typename T> std::string Utils::hex(T t, size_t bits, bool withprefix)
{
    std::stringstream ss;

    if(withprefix && (t > (std::is_signed<T>::value ? 9 : 9u)))
        ss << "0x";

    ss << std::uppercase << std::hex;

    if(bits > 0)
        ss << std::setfill('0') << std::setw(bits / 4);

    if(std::is_signed<T>::value && t < 0)
        ss << "-" << (~t) + 1;
    else
        ss << t;

    return ss.str();
}

template<typename T> size_t Utils::countbits_r(T val)
{
    size_t bits = countbits(val);

    if(bits <= 8) return 8;
    if(bits <= 16) return 16;
    if(bits <= 64) return 64;

    throw std::runtime_error("Bits out of range: " + std::to_string(bits));
}

} // namespace REDasm
