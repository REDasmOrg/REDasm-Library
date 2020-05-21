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
        static constexpr u32 TestValue       = 0x01020304;
        static constexpr u32 Test            = static_cast<const u8&>(TestValue);
        static constexpr bool IsLittleEndian = Test == 0x04;
        static constexpr bool IsBigEndian    = Test == 0x02;

    private:
        template<typename T> static T swap(T hostval) { u8* p = reinterpret_cast<u8*>(&hostval); std::reverse(p, p + sizeof(T)); return hostval; }

        template<typename T> static T fromlittleendian(T hostval) {
            if constexpr(IsBigEndian) return swap<T>(hostval);
            return hostval;
        }

        template<typename T> static T frombigendian(T hostval) {
            if constexpr(IsLittleEndian) return swap<T>(hostval);
            return hostval;
        }

    public:
        Endian() = delete;
        static inline u16 swap16(u16 hostval) { return swap<u16>(hostval); }
        static inline u32 swap32(u32 hostval) { return swap<u32>(hostval); }
        static inline u64 swap64(u64 hostval) { return swap<u64>(hostval); }
        static inline u16 frombigendian16(u16 hostval) { return frombigendian<u16>(hostval); }
        static inline u32 frombigendian32(u32 hostval) { return frombigendian<u32>(hostval); }
        static inline u64 frombigendian64(u64 hostval) { return frombigendian<u64>(hostval); }
        static inline u16 fromlittleendian16(u16 hostval) { return fromlittleendian<u16>(hostval); }
        static inline u32 fromlittleendian32(u32 hostval) { return fromlittleendian<u32>(hostval); }
        static inline u64 fromlittleendian64(u64 hostval) { return fromlittleendian<u64>(hostval); }

    static_assert(Endian::IsLittleEndian != Endian::IsBigEndian, "Unsupported platform endianness");
};
