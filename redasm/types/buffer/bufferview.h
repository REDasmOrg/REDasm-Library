#ifndef BUFFERVIEW_H
#define BUFFERVIEW_H

#include <type_traits>
#include <stdexcept>
#include <iterator>
#include <cstring>
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

    private:
        static const std::string WILDCARD_BYTE;

        template<typename T> struct SearchObject {
            SearchObject(): view(nullptr), result(nullptr) { }
            SearchObject(const BufferView* view, u64 searchsize): view(view), result(nullptr), position(0), searchsize(searchsize) { }
            bool hasNext() const { return view && result; }

            const BufferView* view;
            const T* result;
            u64 position, searchsize;
        };

    public:
        template<typename T> struct SearchResult: public SearchObject<T> {
            SearchResult(): SearchObject<T>(), searchdata(nullptr) { }
            SearchResult(const BufferView* view, const u8* searchdata, u64 searchsize): SearchObject<T>(view, searchsize), searchdata(searchdata) { }
            SearchResult<T> next() const { return this->view->template find<T>(searchdata, this->searchsize, this->position + this->searchsize); }

            const u8* searchdata;
        };

        template<typename T> struct WildcardResult: public SearchObject<T> {
            WildcardResult(): SearchObject<T>() { }
            WildcardResult(const BufferView* view, const std::string& searchwildcard, u64 searchsize): SearchObject<T>(view, searchsize), searchwildcard(searchwildcard) { }
            WildcardResult<T> next() const { return this->view->template wildcard<T>(searchwildcard, this->position + this->searchsize); }

            std::string searchwildcard;
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
        constexpr bool inRange(u64 offset) const { return (offset >= m_offset) && (offset < (m_offset + m_size)); }
        constexpr bool eob() const { return !m_buffer || !this->data() || !m_size; }
        constexpr u64 size() const { return m_size; }
        u8 operator *() const { return *this->data(); }
        template<typename T> WildcardResult<T> wildcard(std::string pattern, u64 startoffset = 0) const;
        template<typename T> SearchResult<T> find(const std::string& s, u64 startoffset = 0) const;
        template<typename T> SearchResult<T> find(const T* pack, u64 startoffset = 0) const;
        template<typename T> SearchResult<T> find(const std::initializer_list<u8> initlist, u64 startoffset = 0) const;
        template<typename T> iterator<T> begin() const { return iterator<T>(this->data()); }
        template<typename T> iterator<T> end() const { return iterator<T>(this->endData()); }
        template<typename T> bool inRange(const T* ptr) const { return (reinterpret_cast<const u8*>(ptr) >= this->data()) && (reinterpret_cast<const u8*>(ptr) < this->endData()); }
        template<typename T> explicit constexpr operator T*() const { return reinterpret_cast<T*>(this->data()); }
        template<typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type> constexpr operator T() const { return *reinterpret_cast<const T*>(this->data()); }
        template<typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type> BufferView operator +(T rhs) const;
        template<typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type> BufferView& operator =(T rhs);
        template<typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type> BufferView& operator +=(T rhs);

    private:
        template<typename T> SearchResult<T> find(const u8* searchdata, size_t searchsize, u64 startoffset = 0) const;
        size_t patternLength(const std::string& pattern) const;
        std::pair<u8, u8> patternRange(std::string &pattern, u64& startoffset, u64& endoffset, u64 &beginoffset) const;
        bool comparePattern(const std::string& pattern, const u8* pdata) const;
        bool preparePattern(std::string& pattern) const;
        u8* endData() const { return this->data() ? (this->data() + this->size()) : nullptr; }

    protected:
        const AbstractBuffer* m_buffer;
        u64 m_offset, m_size;
};

template<typename T> BufferView::WildcardResult<T> BufferView::wildcard(std::string pattern, u64 startoffset) const
{
    if(!this->preparePattern(pattern))
        return WildcardResult<T>();

    u64 beginoffset = 0, endoffset = startoffset, searchsize = this->patternLength(pattern);
    auto bp = this->patternRange(pattern, startoffset, endoffset, beginoffset);

    WildcardResult<T> r(this, pattern, searchsize);
    const u8* pdata = this->data() + startoffset;

    while(pdata < (this->endData() - searchsize))
    {
        if((*pdata != bp.first) && ((*pdata + searchsize) != bp.second))
        {
            pdata++;
            continue;
        }

        if(this->comparePattern(pattern, pdata))
        {
            r.result = reinterpret_cast<T*>(pdata);
            r.position = (pdata - this->data()) - beginoffset;
            break;
        }

        pdata++;
    }

    return r;
}

template<typename T> BufferView::SearchResult<T> BufferView::find(const T *pack, u64 startoffset) const
{
    if(std::is_same<T, char>::value)
        return this->find<T>(reinterpret_cast<const u8*>(pack), std::strlen(reinterpret_cast<const char*>(pack)), startoffset);

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
