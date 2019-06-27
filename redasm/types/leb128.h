#pragma once

#include "base.h"

namespace REDasm {

class LEB128
{
    public:
        LEB128() = delete;
        template<typename T> static T unsignedOf(u8* data, u8** res = nullptr);
        template<typename T> static T unsigned1COf(u8* data, u8** res = nullptr) { return LEB128::unsignedOf<T>(data, res) - 1; }
        template<typename T> static T signedOf(u8* data, u8** res = nullptr) { return static_cast<typename REDasm::signed_of<T>::type>(LEB128::unsignedOf<T>(data, res)); }
};

template<typename T> T LEB128::unsignedOf(u8 *data, u8** res)
{
    size_t i = 0;
    T value = 0;

    while(*data & 0x80)
    {
        value |= ((*data & 0x7F) << (i * 7));
        data++;
        i++;
    }

    value |= ((*data & 0x7F) << (i * 7));
    data++;

    if(res) *res = data;
    return value;
}

} // namespace REDasm
