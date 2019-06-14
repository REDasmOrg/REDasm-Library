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
        WildcardSearchResultImpl(const BufferView* view, const std::string& searchwildcard, size_t searchsize);
        WildcardSearchResult next() const;

    private:
        std::string m_searchwildcard;

    friend class BufferViewImpl;
};

class BufferViewImpl
{
    PIMPL_DECLARE_Q(BufferView)
    PIMPL_DECLARE_PUBLIC(BufferView)

    public:
        BufferViewImpl(BufferView* q);
        size_t patternLength(const std::string& pattern) const;
        std::pair<u8, u8> patternRange(std::string &pattern, size_t& startoffset, size_t& endoffset, size_t &beginoffset) const;
        bool comparePattern(const std::string& pattern, const u8* pdata) const;
        bool preparePattern(std::string& pattern) const;
        u8* endData() const;

    public:
        WildcardSearchResult wildcard(std::string pattern, size_t startoffset = 0) const;
        SearchResult find(const u8* searchdata, size_t searchsize, size_t startoffset = 0) const;

    public:
        static const std::string WILDCARD_BYTE;

    private:
        const AbstractBuffer* m_buffer;
        size_t m_offset, m_size;
};

} // namespace REDasm
