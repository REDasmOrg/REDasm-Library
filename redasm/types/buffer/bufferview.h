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

        template<typename T> struct SearchResult {
            SearchResult(): view(NULL), result(NULL), searchdata(NULL), position(0), searchsize(0) { }
            SearchResult(const BufferView* view, const u8* searchdata, u64 searchsize): view(view), result(NULL), searchdata(searchdata), position(0), searchsize(searchsize) { }
            bool hasNext() const { return view && result; }
            SearchResult<T> next() const { return view->find<T>(searchdata, searchsize, position + searchsize); }

            const BufferView* view;
            const u8* searchdata;
            const T* result;
            u64 position, searchsize;
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
        iterator<u8> end() const { return iterator<u8>(this->endData()); }
        u8* data() const { return m_buffer->data() + m_offset; }
        constexpr const AbstractBuffer* buffer() const { return m_buffer; }
        constexpr bool eob() const { return !m_buffer || !this->data() || !m_size; }
        constexpr u64 size() const { return m_size; }
        u8 operator *() const { return *this->data(); }
        template<typename T> SearchResult<T> find(const std::string& s, u64 startoffset = 0) const;
        template<typename T> SearchResult<T> find(const T* pack, u64 startoffset = 0) const;
        template<typename T> SearchResult<T> find(const std::initializer_list<u8> initlist, u64 startoffset = 0) const;
        template<typename T> iterator<T> begin() const { return iterator<T>(this->data()); }
        template<typename T> iterator<T> end() const { return iterator<T>(this->endData()); }
        template<typename T> explicit constexpr operator T*() const { return reinterpret_cast<T*>(this->data()); }
        template<typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type> constexpr operator T() const { return *reinterpret_cast<const T*>(this->data()); }
        template<typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type> BufferView operator +(T rhs) const;
        template<typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type> BufferView& operator =(T rhs);
        template<typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type> BufferView& operator +=(T rhs);

    private:
        template<typename T> SearchResult<T> find(const u8* searchdata, size_t searchsize, u64 startoffset = 0) const;
        u8* endData() const { return this->data() ? (this->data() + this->size()) : NULL; }


    protected:
        const AbstractBuffer* m_buffer;
        u64 m_offset, m_size;
};

template<typename T> BufferView::SearchResult<T> BufferView::find(const T *pack, u64 startoffset) const
{
    return this->find<T>(reinterpret_cast<const u8*>(pack), sizeof(T), startoffset);
}

template<typename T> BufferView::SearchResult<T> BufferView::find(const std::string &s, u64 startoffset) const
{
    return this->find<T>(reinterpret_cast<const u8*>(s.c_str()), s.size(), startoffset);
}

template<typename T> BufferView::SearchResult<T> BufferView::find(const std::initializer_list<u8> initlist, u64 startoffset) const
{
    return this->find<T>(initlist.begin(), initlist.size(), startoffset);
}

template<typename T> BufferView::SearchResult<T> BufferView::find(const u8* searchdata, size_t searchsize, u64 startoffset) const
{
    if(this->eob() || !searchdata || !searchsize || (searchsize > this->size()))
        return SearchResult<T>();

    SearchResult<T> r(this, searchdata, searchsize);
    const u8* pdata = this->data() + startoffset;

    while(pdata < (this->endData() - searchsize))
    {
        if(!std::equal(pdata, pdata + searchsize, searchdata))
        {
            pdata++;
            continue;
        }

        r.result = reinterpret_cast<const T*>(pdata);
        r.position = pdata - this->data();
        break;
    }

    return r;
}

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
