#pragma once

//Based on: https://stackoverflow.com/questions/28675727/using-crc32-algorithm-to-hash-string-at-compile-time

#include "string.h"
#include "../types/base.h"

namespace REDasm {
namespace Detail {

template <unsigned c, int k = 8> struct CRC32GenTable : CRC32GenTable<((c & 1) ? 0xedb88320 : 0) ^ (c >> 1), k - 1> { };
template <unsigned c> struct CRC32GenTable<c, 0> { enum {value = c}; };

#define A(x) B(x) B(x + 128)
#define B(x) C(x) C(x +  64)
#define C(x) D(x) D(x +  32)
#define D(x) E(x) E(x +  16)
#define E(x) F(x) F(x +   8)
#define F(x) G(x) G(x +   4)
#define G(x) H(x) H(x +   2)
#define H(x) I(x) I(x +   1)
#define I(x) CRC32GenTable<x>::value ,

constexpr unsigned CRC32_TABLE[] = { A(0) };

// Constexpr implementation and helpers
template<typename T> constexpr u32 static_crc32_impl(const T* data, size_t len, u32 crc) { return len ? static_crc32_impl(data + 1, len - 1, (crc >> 8) ^ CRC32_TABLE[(crc & 0xFF) ^ *data]) : crc; }

} // namespace Detail

template<typename T> constexpr u32 static_crc32(const T* data, size_t length) { return ~Detail::static_crc32_impl(data, length, ~0); }
constexpr u32 static_crc32(const char* s) { return ~Detail::static_crc32_impl(s, static_strlen(s), ~0); }

} // namespace REDasm
