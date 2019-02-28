#ifndef ABSTRACTBUFFER_H
#define ABSTRACTBUFFER_H

#include <algorithm>
#include <memory>
#include "../endianness/endianness.h"
#include "../base_types.h"

namespace REDasm {
namespace Buffer {

class BufferView;

class AbstractBuffer
{
    public:
        virtual ~AbstractBuffer() = default;
        u8& at(u64 idx);
        u8 at(u64 idx) const;
        bool empty() const;
        BufferView view() const;
        BufferView view(u64 offset, u64 size = 0) const;
        virtual void resize(u64 size) = 0;
        virtual u8* data() const = 0;
        virtual u64 size() const = 0;
        u8& operator[](u64 idx);
        u8 operator[](u64 idx) const;

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

} // namespace Buffer

using AbstractBuffer = Buffer::AbstractBuffer;

} // namespace REDasm

#endif // ABSTRACTBUFFER_H
