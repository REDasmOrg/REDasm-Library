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
        template<typename T> static T tolittlendian(T v) { return IsLittleEndian ? swap<T>(v) : v; }
        template<typename T> static T tobigendian(T v) { return IsBigEndian ? swap<T>(v) : v; }

    public:
        Endian() = delete;
        static inline u16 swap16(u16 v) { return swap<u16>(v); }
        static inline u32 swap32(u32 v) { return swap<u32>(v); }
        static inline u64 swap64(u64 v) { return swap<u64>(v); }
        static inline u16 tobigendian16(u16 v) { return tobigendian<u16>(v); }
        static inline u32 tobigendian32(u32 v) { return tobigendian<u32>(v); }
        static inline u64 tobigendian64(u64 v) { return tobigendian<u64>(v); }
        static inline u16 tolittlendian16(u16 v) { return tolittlendian<u16>(v); }
        static inline u32 tolittlendian32(u32 v) { return tolittlendian<u32>(v); }
        static inline u64 tolittlendian64(u64 v) { return tolittlendian<u64>(v); }

    static_assert(Endian::IsLittleEndian != Endian::IsBigEndian, "Unsupported platform endianness");
};
