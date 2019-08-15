#pragma once

#include "../types/endianness/endianness.h"
#include "../types/object.h"
#include "../types/string.h"
#include "../types/base.h"
#include "../macros.h"

namespace REDasm {

class BufferView;

class LIBREDASM_API AbstractBuffer: public Object
{
    public:
        virtual ~AbstractBuffer() = default;
        u8& at(size_t idx);
        u8 at(size_t idx) const;
        bool empty() const;
        BufferView view() const;
        BufferView view(size_t offset, size_t size = REDasm::npos) const;
        u8& operator[](size_t idx);
        u8 operator[](size_t idx) const;
        virtual void resize(size_t size) = 0;
        virtual u8* data() const = 0;
        virtual size_t size() const = 0;

    public:
        template<typename T> explicit constexpr operator T*() const { return reinterpret_cast<T*>(this->data()); }
};

template<typename T, typename InBufferType, typename OutBufferType> void swapEndianness(const InBufferType* inbuffer, OutBufferType* outbuffer, size_t size = 0)
{
    if(!size)
        size = inbuffer->size();

    size = std::min(size, inbuffer->size());

    if(outbuffer->size() < size)
        outbuffer->resize(size);

    std::copy_n(inbuffer->data(), size, outbuffer->data());
    Endianness::swap<T>(outbuffer->data(), size);
}

template<typename T, typename BufferType> void swapEndianness(BufferType* buffer, size_t size = 0)
{
    if(!size)
        size = buffer->size();

    Endianness::swap<T>(buffer->data(), size);
}

} // namespace REDasm
