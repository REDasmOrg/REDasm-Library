#pragma once

#include <redasm/buffer/abstractbuffer.h>
#include <redasm/buffer/bufferview.h>
#include <redasm/pimpl.h>

namespace REDasm {

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
        template<typename T> BufferView::WildcardResult<T> wildcard(std::string pattern, size_t startoffset = 0) const;
        template<typename T> BufferView::SearchResult<T> find(const u8* searchdata, size_t searchsize, size_t startoffset = 0) const;

    private:
        const AbstractBuffer* m_buffer;
        size_t m_offset, m_size;
};

template<typename T> BufferView::WildcardResult<T> BufferViewImpl::wildcard(std::string pattern, size_t startoffset) const
{
    PIMPL_Q(const BufferView);

    if(!this->preparePattern(pattern))
        return BufferView::WildcardResult<T>();

    size_t beginoffset = 0, endoffset = startoffset, searchsize = this->patternLength(pattern);
    auto bp = this->patternRange(pattern, startoffset, endoffset, beginoffset);

    BufferView::WildcardResult<T> r(this, pattern, searchsize);
    const u8* pdata = q->data() + startoffset;

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
            r.position = (pdata - q->data()) - beginoffset;
            break;
        }

        pdata++;
    }

    return r;
}

template<typename T> BufferView::SearchResult<T> BufferViewImpl::find(const u8 *searchdata, size_t searchsize, size_t startoffset) const
{
    PIMPL_Q(const BufferView);

    if(q->eob() || !searchdata || !searchsize || (searchsize > q->size()))
        return BufferView::SearchResult<T>();

    BufferView::SearchResult<T> r(this, searchdata, searchsize);
    const u8* pdata = q->data() + startoffset;

    while((pdata + searchsize) < this->endData())
    {
        if(!std::equal(pdata, pdata + searchsize, searchdata))
        {
            pdata++;
            continue;
        }

        r.result = reinterpret_cast<const T*>(pdata);
        r.position = pdata - q->data();
        break;
    }

    return r;
}


} // namespace REDasm
