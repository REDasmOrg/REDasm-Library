#pragma once

#include <type_traits>
#include <cstring>
#include "../types/base.h"
#include "../types/endianness/endianness.h"
#include "../pimpl.h"
#include "abstractbuffer.h"

namespace REDasm {

class SearchResultImpl;

class SearchResult
{
    PIMPL_DECLARE_P(SearchResult)
    PIMPL_DECLARE_PRIVATE(SearchResult)

    private:
        SearchResult();
        SearchResult(const BufferView* view, const u8* searchdata, size_t searchsize);

    public:
        SearchResult next() const;
        size_t position() const;
        bool isValid() const;
        const u8 *result() const;
        template<typename T> T* pointer() { return reinterpret_cast<T*>(this->result()); }

    friend class BufferViewImpl;
    friend class BufferView;
};

class WildcardSearchResultImpl;

class WildcardSearchResult
{
    PIMPL_DECLARE_P(WildcardSearchResult)
    PIMPL_DECLARE_PRIVATE(WildcardSearchResult)

    private:
        WildcardSearchResult();
        WildcardSearchResult(const BufferView* view, const String& searchwildcard, size_t searchsize);

    public:
        WildcardSearchResult next() const;
        size_t position() const;
        bool isValid() const;
        const u8* result() const;
        template<typename T> T* pointer() { return reinterpret_cast<T*>(this->result()); }

    friend class BufferViewImpl;
    friend class BufferView;
};

class BufferViewImpl;

class LIBREDASM_API BufferView
{
    PIMPL_DECLARE_P(BufferView)
    PIMPL_DECLARE_PRIVATE(BufferView)

    public:
        BufferView();
        BufferView(const BufferView& view);
        BufferView(const AbstractBuffer* buffer, size_t offset, size_t size);
        u8 operator[](size_t idx) const;
        u8& operator[](size_t idx);
        BufferView& operator++();
        BufferView operator ++(int);
        BufferView view(size_t offset, size_t size = 0) const;
        void copyTo(AbstractBuffer* buffer);
        String toString() const;
        void resize(size_t size);
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
        WildcardSearchResult wildcard(const String &pattern, size_t startoffset = 0) const;
        SearchResult find(const u8* searchdata, size_t searchsize, size_t startoffset = 0) const;

    public:
        template<typename T> SearchResult find(const String& s, size_t startoffset = 0) const;
        template<typename T> SearchResult find(const T* pack, size_t startoffset = 0) const;
        template<typename T> SearchResult find(const std::initializer_list<u8> initlist, size_t startoffset = 0) const;
        template<typename T> bool inRange(const T* ptr) const { return (reinterpret_cast<const u8*>(ptr) >= this->data()) && (reinterpret_cast<const u8*>(ptr) < this->endData()); }
        template<typename T> explicit constexpr operator T*() const { return reinterpret_cast<T*>(this->data()); }
        template<typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type> constexpr operator T() const { return *reinterpret_cast<const T*>(this->data()); }
        template<typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type> BufferView& operator =(T rhs);
};

template<typename T> SearchResult BufferView::find(const T *pack, size_t startoffset) const
{
    if(std::is_same<T, char>::value)
        return this->find(reinterpret_cast<const u8*>(pack), std::strlen(reinterpret_cast<const char*>(pack)), startoffset);

    return this->find(reinterpret_cast<const u8*>(pack), sizeof(T), startoffset);
}

template<typename T> SearchResult BufferView::find(const String &s, size_t startoffset) const
{
    return this->find(reinterpret_cast<const u8*>(s.c_str()), s.size(), startoffset);
}

template<typename T> SearchResult BufferView::find(const std::initializer_list<u8> initlist, size_t startoffset) const
{
    return this->find(initlist.begin(), initlist.size(), startoffset);
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
