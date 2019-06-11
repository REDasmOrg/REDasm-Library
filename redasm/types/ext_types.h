#pragma once

#include "base_types.h"

namespace REDasm {

class LEB128
{
    public:
        LEB128() = delete;
        template<typename T> static T unsignedOf(u8* data);
        template<typename T> static T unsigned1COf(u8* data) { return LEB128::unsignedOf<T>(data) - 1; }
        template<typename T> static T signedOf(u8* data) { return static_cast<typename REDasm::signed_of<T>::type>(LEB128::unsignedOf<T>(data)); }
};

template<typename T> T LEB128::unsignedOf(u8 *data)
{
    size_t i = 0;
    T value = 0;

    while(*data & 0x80)
    {
        value |= ((*data & 0x7F) << (i * 7));
        (*data)++;
        i++;
    }

    value |= ((*data & 0x7F) << (i * 7));
    (*data)++;
    return value;
}

} // namespace REDasm
