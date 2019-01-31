#ifndef REDASM_BUFFER_H
#define REDASM_BUFFER_H

#include <string>
#include <vector>
#include <libredasm_export.h>
#include "types/base_types.h"
#include "types/endianness/endianness.h"

namespace REDasm {

class BufferRef;

class Buffer: public std::vector<u8>
{
    friend class BufferRef;

    public:
        Buffer();
        BufferRef slice(u64 offset);
        Buffer createFilled(size_t n, u8 b = 0) const;

    public:
        static Buffer fromFile(const std::string& file);
        static Buffer fill(size_t count, u8 b = 0);
        u8 operator *() const { return this->at(0); }

    public:
        template<typename T> Buffer& swapEndianness(size_t size = -1u);
        template<typename T> Buffer swapEndianness(size_t size = -1u) const;
        template<typename T> constexpr operator T*() const { return reinterpret_cast<T*>(this->data()); }
        template<typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type> constexpr operator T() { return *reinterpret_cast<const T*>(this->data()); }

    public:
        static Buffer invalid;
};

template<typename T> Buffer& Buffer::swapEndianness(size_t size)
{
    if(size == -1u)
        size = this->size();

    Endianness::swap<T>(this->data(), size);
    return *this;
}

template<typename T> Buffer Buffer::swapEndianness(size_t size) const
{
    Buffer buffer = *this;
    return buffer.swapEndianness<T>(size);
}

class BufferRef
{
    public:
        BufferRef();
        BufferRef(Buffer *buffer, u64 offset);
        BufferRef(const BufferRef *buffer, u64 offset);
        BufferRef& advance(int offset);
        BufferRef advance(int offset) const;
        Buffer filled(size_t n, u8 b = 0) const;
        void resize(size_t s);
        bool copyTo(Buffer& buffer);
        bool eob() const;
        bool empty() const;
        size_t size() const;
        u8* data() const;
        u8* data();

    public:
        u8 operator [](size_t idx) const;
        u8 operator *() const { return *m_data; }
        BufferRef operator ++(int) { BufferRef copy(this, 0); this->m_data++; this->m_size--; return copy; }
        BufferRef& operator ++() { this->m_data++; this->m_size--; return *this; }
        template<typename T, typename = typename std::enable_if< std::is_integral<T>::value>::type> constexpr operator T() { return *reinterpret_cast<const T*>(this->data()); }

    public:
        template<typename T> constexpr s64 swapEndianness() { return Endianness::swap<T>(m_data, m_size); }
        template<typename T> constexpr operator T*() const { return reinterpret_cast<T*>(this->data()); }
        template<typename T> BufferRef& operator =(T rhs);

    private:
        const Buffer* m_buffer;
        u8* m_data;
        size_t m_size;
};

template<typename T> BufferRef& BufferRef::operator =(T rhs)
{
    T* p = reinterpret_cast<T*>(m_data);

    if(Endianness::of<T>::needsSwap)
        *p = Endianness::swap<T>(rhs);
    else
        *p = rhs;

    return *this;
}

} // namespace REDasm

#endif // REDASM_BUFFER_H
