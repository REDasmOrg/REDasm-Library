#pragma once

#include <type_traits>
#include <iterator>
#include <cstring>
#include "../types/base_types.h"
#include "../types/endianness/endianness.h"
#include "../pimpl.h"
#include "abstractbuffer.h"

namespace REDasm {

class BufferViewImpl;

class LIBREDASM_API BufferView
{
    PIMPL_DECLARE_P(BufferView)
    PIMPL_DECLARE_PRIVATE(BufferView)

    public:
        template<typename T> struct iterator: public std::iterator<std::forward_iterator_tag, T> {
            explicit iterator(const T* data) { m_data = reinterpret_cast<const T*>(data); }
            iterator<T>& operator++() { m_data++; return *this; }
            iterator<T> operator ++(int) { iterator<T> ret(reinterpret_cast<const T*>(m_data)); m_data++; return ret; }
            constexpr T operator *() const { return *m_data; }
            const T* data() const { return m_data; }
            bool operator ==(const iterator<T>& rhs) const { return m_data == rhs.m_data; }
            bool operator !=(const iterator<T>& rhs) const { return m_data != rhs.m_data; }

            private:
                const T* m_data;
        };

        typedef iterator<u8> byte_iterator;

    private:
        static const std::string WILDCARD_BYTE;

        template<typename T> struct SearchObject {
            SearchObject(): view(nullptr), result(nullptr) { }
            SearchObject(const BufferView* view, size_t searchsize): view(view), result(nullptr), position(0), searchsize(searchsize) { }
            bool hasNext() const { return view && result; }

            const BufferView* view;
            const T* result;
            size_t position, searchsize;
        };

    public:
        template<typename T> struct SearchResult: public SearchObject<T> {
            SearchResult(): SearchObject<T>(), searchdata(nullptr) { }
            SearchResult(const BufferView* view, const u8* searchdata, size_t searchsize): SearchObject<T>(view, searchsize), searchdata(searchdata) { }
            SearchResult<T> next() const { return this->view->template find<T>(searchdata, this->searchsize, this->position + this->searchsize); }

            const u8* searchdata;
        };

        template<typename T> struct WildcardResult: public SearchObject<T> {
            WildcardResult(): SearchObject<T>() { }
            WildcardResult(const BufferView* view, const std::string& searchwildcard, size_t searchsize): SearchObject<T>(view, searchsize), searchwildcard(searchwildcard) { }
            WildcardResult<T> next() const { return this->view->template wildcard<T>(searchwildcard, this->position + this->searchsize); }

            std::string searchwildcard;
        };

    public:
        BufferView();
        BufferView(const AbstractBuffer* buffer, size_t offset, size_t size);
        u8 operator[](size_t idx) const;
        u8& operator[](size_t idx);
        BufferView& operator++();
        BufferView operator ++(int);
        BufferView view(size_t offset, size_t size = 0) const;
        void copyTo(AbstractBuffer* buffer);
        std::string toString() const;
        void resize(size_t size);
        byte_iterator begin() const;
        byte_iterator end() const;
        u8* data() const;
        u8* endData() const;
        const AbstractBuffer* buffer() const;
        bool inRange(size_t offset) const;
        bool eob() const;
        offset_t offset() const;
        size_t size() const;
        u8 operator *() const;
        BufferView operator +(size_t rhs) const;
        BufferView& operator +=(size_t rhs);
        template<typename T> WildcardResult<T> wildcard(std::string pattern, size_t startoffset = 0) const;
        template<typename T> SearchResult<T> find(const std::string& s, size_t startoffset = 0) const;
        template<typename T> SearchResult<T> find(const T* pack, size_t startoffset = 0) const;
        template<typename T> SearchResult<T> find(const std::initializer_list<u8> initlist, size_t startoffset = 0) const;
        template<typename T> iterator<T> begin() const { return iterator<T>(this->data()); }
        template<typename T> iterator<T> end() const { return iterator<T>(this->endData()); }
        template<typename T> bool inRange(const T* ptr) const { return (reinterpret_cast<const u8*>(ptr) >= this->data()) && (reinterpret_cast<const u8*>(ptr) < this->endData()); }
        template<typename T> explicit constexpr operator T*() const { return reinterpret_cast<T*>(this->data()); }
        template<typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type> constexpr operator T() const { return *reinterpret_cast<const T*>(this->data()); }
        template<typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type> BufferView& operator =(T rhs);
};

template<typename T> BufferView::WildcardResult<T> BufferView::wildcard(std::string pattern, size_t startoffset) const
{
    //return pimpl_p()->template<T> wildcard(pattern, startoffset);
}

template<typename T> BufferView::SearchResult<T> BufferView::find(const T *pack, size_t startoffset) const
{
    if(std::is_same<T, char>::value)
        return this->find<T>(reinterpret_cast<const u8*>(pack), std::strlen(reinterpret_cast<const char*>(pack)), startoffset);

    return this->find<T>(reinterpret_cast<const u8*>(pack), sizeof(T), startoffset);
}

template<typename T> BufferView::SearchResult<T> BufferView::find(const std::string &s, size_t startoffset) const
{
    return this->find<T>(reinterpret_cast<const u8*>(s.c_str()), s.size(), startoffset);
}

template<typename T> BufferView::SearchResult<T> BufferView::find(const std::initializer_list<u8> initlist, size_t startoffset) const
{
    return this->find<T>(initlist.begin(), initlist.size(), startoffset);
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

} // namespace REDasm
