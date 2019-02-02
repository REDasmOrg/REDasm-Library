#ifndef BUFFERVIEW_H
#define BUFFERVIEW_H

#include <type_traits>
#include <stdexcept>
#include <iterator>
#include "../base_types.h"
#include "../endianness/endianness.h"
#include "abstractbuffer.h"

namespace REDasm {
namespace Buffer {

class BufferView
{
    public:
        template<typename T> struct iterator: public std::iterator<std::forward_iterator_tag, u8> {
            explicit iterator(u8* data) { m_data = reinterpret_cast<T*>(data); }
            iterator<T>& operator++() { m_data++; return *this; }
            iterator<T> operator ++(int) { iterator<T> ret(reinterpret_cast<u8*>(m_data)); m_data++; return ret; }
            constexpr T operator *() const { return *m_data; }
            bool operator ==(const iterator<T>& rhs) const { return m_data == rhs.m_data; }
            bool operator !=(const iterator<T>& rhs) const { return m_data != rhs.m_data; }

            private:
                T* m_data;
        };

    public:
        BufferView();
        BufferView(const AbstractBuffer* buffer, u64 offset, u64 size);
        u8 operator[](size_t idx) const;
        u8& operator[](size_t idx);
        BufferView& operator++();
        BufferView operator ++(int);
        BufferView view(u64 offset, u64 size = 0) const;
        void copyTo(AbstractBuffer* buffer);
        void resize(u64 size);
        iterator<u8> begin() const { return iterator<u8>(this->data()); }
        iterator<u8> end() const { return iterator<u8>(this->data() + this->size()); }
        u8* data() const { return m_buffer->data() + m_offset; }
        constexpr const AbstractBuffer* buffer() const { return m_buffer; }
        constexpr bool eob() const { return !m_buffer || !this->data() || !m_size; }
        constexpr u64 size() const { return m_size; }
        u8 operator *() const { return *this->data(); }
        template<typename T> iterator<T> begin() const { return iterator<T>(this->data()); }
        template<typename T> iterator<T> end() const { return iterator<T>(this->data() + this->size()); }
        template<typename T> explicit constexpr operator T*() const { return reinterpret_cast<T*>(this->data()); }
        template<typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type> constexpr operator T() const { return *reinterpret_cast<const T*>(this->data()); }
        template<typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type> BufferView operator +(T rhs) const;
        template<typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type> BufferView& operator =(T rhs);
        template<typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type> BufferView& operator +=(T rhs);

    protected:
        const AbstractBuffer* m_buffer;
        u64 m_offset, m_size;
};

template<typename T, typename> BufferView BufferView::operator +(T offset) const
{
    if(offset > m_size)
        throw std::out_of_range("Buffer:operator+=(): offset > size");

    return BufferView(m_buffer, m_offset + offset, m_size - offset);
}

template<typename T, typename> BufferView &BufferView::operator +=(T offset)
{
    if(offset > m_size)
        throw std::out_of_range("Buffer:operator+=(): offset > size");

    m_offset += offset;
    m_size -= offset;
    return *this;
}

template<typename T, typename > BufferView &BufferView::operator =(T rhs)
{
    T* p = reinterpret_cast<T*>(this->data());

    if(Endianness::of<T>::needsSwap)
        *p = Endianness::swap<T>(rhs);
    else
        *p = rhs;

    return *this;
}

} // namespace Buffer

using BufferView = Buffer::BufferView;

} // namespace REDasm

#endif // BUFFERVIEW_H
