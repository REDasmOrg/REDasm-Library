#ifndef ENDIANNESS_BASE_H
#define ENDIANNESS_BASE_H

#include "../base_types.h"
#include <algorithm>

namespace REDasm {

typedef u32 endianness_t;

namespace Endianness {

enum { LittleEndian = 0, BigEndian = 1 };

namespace Impl { // https://stackoverflow.com/questions/1583791/constexpr-and-endianness

static constexpr u32 TestValue         = 0x01020304;
static constexpr u32 Test              = static_cast<const u8&>(TestValue);
static constexpr bool TestLittleEndian = Test == 0x04;
static constexpr bool TestBigEndian    = Test == 0x02;

static_assert(TestLittleEndian != TestBigEndian, "Unsupported platform endianness");

} // namespace Impl

constexpr bool isLittleEndian = Impl::TestLittleEndian;
constexpr bool isBigEndian = Impl::TestBigEndian;
constexpr int current = Endianness::isLittleEndian ? Endianness::LittleEndian : Endianness::BigEndian;

template<typename T> T swap(T v) { u8* p = reinterpret_cast<u8*>(&v); std::reverse(p, p + sizeof(T)); return v; }

} // namespace Endianness
} // namespace REDasm

#endif // ENDIANNESS_BASE_H
