#pragma once

#if _MSC_VER
    #pragma warning(disable: 4309)
#endif //_MSC_VER

#include <algorithm>

namespace REDasm {

typedef size_t endianness_t;

namespace Endianness {

enum { LittleEndian = 0, BigEndian = 1 };

namespace Detail { // https://stackoverflow.com/questions/1583791/constexpr-and-endianness

static constexpr size_t TestValue      = 0x01020304;
static constexpr size_t Test           = static_cast<const unsigned char&>(TestValue);
static constexpr bool TestLittleEndian = Test == 0x04;
static constexpr bool TestBigEndian    = Test == 0x02;

static_assert(TestLittleEndian != TestBigEndian, "Unsupported platform endianness");

} // namespace Detail

constexpr bool isLittleEndian = Detail::TestLittleEndian;
constexpr bool isBigEndian = Detail::TestBigEndian;
constexpr int current = Endianness::isLittleEndian ? Endianness::LittleEndian : Endianness::BigEndian;

template<typename T> T swap(T v) { unsigned char* p = reinterpret_cast<unsigned char*>(&v); std::reverse(p, p + sizeof(T)); return v; }

} // namespace Endianness
} // namespace REDasm
