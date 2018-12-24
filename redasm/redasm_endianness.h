#ifndef REDASM_ENDIANNESS_H
#define REDASM_ENDIANNESS_H

#include <algorithm>
#include "redasm_types.h"

namespace REDasm {

typedef u32 endianness_t;

namespace Endianness {

enum { LittleEndian = 0, BigEndian = 1, };

int current();
inline bool needsSwap(endianness_t endianness) { return Endianness::current() != endianness; }

template<typename T> T swap(T v) {
   u8* p = reinterpret_cast<u8*>(&v);
   std::reverse(p, p + sizeof(T));
   return v;
}

template<typename T> T checkSwap(T v, endianness_t endianness) {
   if(!Endianness::needsSwap(endianness))
       return v;

   return Endianness::swap(v);
}

template<typename T> size_t swap(u8* data, size_t size) {
    T* pendingdata = reinterpret_cast<T*>(data);
    s64 pendingsize = static_cast<s64>(size);

    while(pendingsize >= sizeof(T))
    {
        *pendingdata = Endianness::swap<T>(*pendingdata);
        pendingdata++;
        pendingsize -= sizeof(T);
    }

    return pendingsize;
}

template<typename T> T cfbe(T v) { return checkSwap(v, Endianness::BigEndian); }                // Convert FROM BigEndian TO PlatformEndian
template<typename T> T cfle(T v) { return checkSwap(v, Endianness::LittleEndian); }             // Convert FROM LittleEndian TO PlatformEndian
template<typename T> T cfbe(const T* data) { return cfbe(*data); }
template<typename T> T cfle(const T* data) { return cfle(*data); }

} // namespace Endianness
} // namespace REDasm

#endif // ENDIANNESS_H
