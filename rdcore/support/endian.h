#pragma once

// https://stackoverflow.com/questions/1583791/constexpr-and-endianness

#if _MSC_VER
    #pragma warning(disable: 4309)
#endif //_MSC_VER

#include <rdapi/types.h>
#include <algorithm>

class Endian
{
    private:
        static constexpr u32 TestValue         = 0x01020304;
        static constexpr u32 Test              = static_cast<const u8&>(TestValue);
        static constexpr bool IsLittleEndian   = Test == 0x04;
        static constexpr bool IsBigEndian      = Test == 0x02;

    private:
        template<typename T> static T swap(T v) { u8* p = reinterpret_cast<u8*>(&v); std::reverse(p, p + sizeof(T)); return v; }
        template<typename T> static T swapbe(T v) { return IsLittleEndian ? swap<T>(v) : v; }
        template<typename T> static T swaple(T v) { return IsBigEndian ? swap<T>(v) : v; }

    public:
        Endian() = delete;
        static inline u16 swap16(u16 v) { return swap<u16>(v); }
        static inline u32 swap32(u32 v) { return swap<u32>(v); }
        static inline u64 swap64(u64 v) { return swap<u64>(v); }
        static inline u16 swap16le(u16 v) { return swaple<u16>(v); }
        static inline u32 swap32le(u32 v) { return swaple<u32>(v); }
        static inline u64 swap64le(u64 v) { return swaple<u64>(v); }
        static inline u16 swap16be(u16 v) { return swapbe<u16>(v); }
        static inline u32 swap32be(u32 v) { return swapbe<u32>(v); }
        static inline u64 swap64be(u64 v) { return swapbe<u64>(v); }

    static_assert(Endian::IsLittleEndian != Endian::IsBigEndian, "Unsupported platform endianness");
};
