#pragma once

#include <type_traits>
#include <sstream>
#include <string>
#include <iomanip>
#include <climits>
#include <cmath>
#include <cassert>
#include "demangler.h"
#include "../types/base_types.h"
#include "../types/buffer/memorybuffer.h"
#include "../types/buffer/bufferview.h"

namespace REDasm
{

bool isPath(const std::string& s);
std::string simplified(std::string s);
std::string ltrimmed(std::string s);
std::string rtrimmed(std::string s);
std::string trimmed(std::string s);
std::string pathext(const std::string& s);
inline bool pathext_is(const std::string& s, const std::string& ext) { return pathext(s) == ext; }
inline bool startsWith(const std::string& s, const std::string& b) { return s.find( b) == 0; }
inline bool endsWith(const std::string& s, const std::string& e) { return s.size() >= e.size() && s.compare(s.size() - e.size(), e.size(), e) == 0; }
inline std::string quoted_s(const std::string& s) {  return "'" + s + "'"; }
inline std::string quoted(const std::string& s) {  return "\"" + s + "\""; }
inline std::string quoted(const char* s) { return REDasm::quoted(std::string(s)); }
inline std::string quoted_s(const char* s) { return REDasm::quoted_s(std::string(s)); }
std::string wtoa(const std::wstring& wide);
std::string hexstring(const char* data, size_t size);
inline std::string hexstring(const BufferView& view, size_t size) { return hexstring(static_cast<const char*>(view), size); }
MemoryBuffer bytes(const std::string& s, u64 offset = 0, u64 hexlen = 0);
bool byte(const std::string& s, u8* val, u64 offset = 0);
inline std::string trampoline(const std::string& s, const std::string& prefix = std::string()) { return prefix + "_" + s; }

template<typename T> struct bitwidth { static constexpr T value = sizeof(T) * CHAR_BIT; };
template<typename T> inline std::string quoted(T t) { return REDasm::quoted(std::to_string(t)); }
template<typename T, typename U> inline T* relpointer(U* base, size_t offset) { return reinterpret_cast<T*>(reinterpret_cast<size_t>(base) + offset); }

template<typename Container> std::string join(const Container& c, const std::string& sep) {
    std::stringstream ss;

    for(auto it = c.begin(); it != c.end(); it++) {
        if(it != c.begin())
            ss << sep;

        ss << *it;
    }

    return ss.str();
}

template<typename T> T unmask(T val, T mask) {
    T result = 0;

    for(T i = 0, j = 0; i < bitwidth<T>::value; i++) {
        if(!(mask & (1 << i))) // Check if mask bit is set
            continue;

        if(val & (1 << i)) // Check if data bit is set
            result |= (1 << j);

        j++;
    }

    return result;
}

template<typename T, typename U> inline T readpointer(U** p) {
    T v = *reinterpret_cast<T*>(*p);
    *p = REDasm::relpointer<U>(*p, sizeof(T));
    return v;
}

template<typename T, typename ST = typename signed_of<T>::type> ST twoc(T val) { return static_cast<ST>((~val) + 1); }

template<typename T, typename U> T rol(T n, U c) {
    assert(c < bitwidth<T>::value);

    if(!c)
        return n;

    return (n << c) | (n >> (bitwidth<T>::value - c));
}

template<typename T, typename U> T ror(T n, U c) {
    assert(c < bitwidth<T>::value);

    if(!c)
        return n;

    return (n >> c) | (n << (bitwidth<T>::value - c));
}

template<typename T, typename U> T aligned(T t, U a) {
    T r = t % a;
    return r ? (t + (a - r)) : t;
}

template<typename T> std::string wtoa(T* ws, size_t len) {
    std::string s;
    char* p = reinterpret_cast<char*>(ws);

    for(size_t i = 0; i < len; i++, p += sizeof(char) * 2)
        s += *p;

    return s;
}

template<typename T> std::string dec(T t) {
    std::stringstream ss;
    ss << t;
    return ss.str();
}

template<typename T> std::string hex(T t, u64 bits = 0, bool withprefix = false) {
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

template<typename T> u64 countbits(T val) {
    double bytes = std::log(static_cast<double>(val)) / std::log(256.0);
    return static_cast<u64>(std::ceil(bytes)) * 8;
}

template<typename T> u64 countbits_r(T val) {
    u64 bits = countbits(val);

    if(bits <= 8)
        return 8;
    if(bits <= 16)
        return 16;
    if(bits <= 64)
        return 64;

    throw std::runtime_error("Bits out of range: " + std::to_string(bits));
}

inline std::string uniquename(const std::string& s, address_t address) { return s + "_"  + REDasm::hex(address); }

}
