#pragma once

#include <redasm/buffer/abstractbuffer.h>
#include <redasm/buffer/bufferview.h>
#include <redasm/pimpl.h>

namespace REDasm {

class SearchObjectImpl
{
    public:
        SearchObjectImpl();
        SearchObjectImpl(const BufferView* view, size_t searchsize);
        bool hasNext() const;
        size_t position() const;
        const u8* result() const;

    protected:
        const BufferView* m_view;
        const u8* m_result;
        size_t m_position, m_searchsize;

    friend class BufferViewImpl;
};

class SearchResultImpl: public SearchObjectImpl
{
    public:
        SearchResultImpl();
        SearchResultImpl(const BufferView* view, const u8* searchdata, size_t searchsize);
        SearchResult next() const;

    private:
        const u8* m_searchdata;

    friend class BufferViewImpl;
};

class WildcardSearchResultImpl: public SearchObjectImpl
{
    public:
        WildcardSearchResultImpl();
        WildcardSearchResultImpl(const BufferView* view, const String& searchwildcard, size_t searchsize);
        WildcardSearchResult next() const;

    private:
        String m_searchwildcard;

    friend class BufferViewImpl;
};

class BufferViewImpl
{
    PIMPL_DECLARE_Q(BufferView)
    PIMPL_DECLARE_PUBLIC(BufferView)

    public:
        BufferViewImpl(BufferView* q);
        BufferViewImpl(BufferView* q, const AbstractBuffer *buffer, size_t offset, size_t size);
        size_t patternLength(const String& pattern) const;
        std::pair<u8, u8> patternRange(String &pattern, size_t& startoffset, size_t& endoffset, size_t &beginoffset) const;
        bool comparePattern(const String& pattern, const u8* pdata) const;
        bool preparePattern(String& pattern) const;
        u8* endData() const;

    public:
        WildcardSearchResult wildcard(String pattern, size_t startoffset = 0) const;
        SearchResult find(const u8* searchdata, size_t searchsize, size_t startoffset = 0) const;

    public:
        static const String WILDCARD_BYTE;

    private:
        const AbstractBuffer* m_buffer;
        size_t m_offset, m_size;
};

} // namespace REDasm
