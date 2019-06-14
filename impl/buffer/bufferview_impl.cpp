#include "bufferview_impl.h"
#include <redasm/support/utils.h>

namespace REDasm {

SearchObjectImpl::SearchObjectImpl(): m_view(nullptr), m_result(nullptr) { }
SearchObjectImpl::SearchObjectImpl(const BufferView *view, size_t searchsize): m_view(view), m_result(nullptr), m_position(0), m_searchsize(searchsize) { }
bool SearchObjectImpl::hasNext() const { return m_view && m_result; }
size_t SearchObjectImpl::position() const { return m_position; }
const u8 *SearchObjectImpl::result() const { return m_result; }
SearchResultImpl::SearchResultImpl(): SearchObjectImpl(), m_searchdata(nullptr) { }
SearchResultImpl::SearchResultImpl(const BufferView *view, const u8 *searchdata, size_t searchsize): SearchObjectImpl(view, searchsize), m_searchdata(searchdata) { }
SearchResult SearchResultImpl::next() const { return m_view->find(m_searchdata, m_searchsize, m_position + m_searchsize); }

WildcardSearchResultImpl::WildcardSearchResultImpl(): SearchObjectImpl() { }
WildcardSearchResultImpl::WildcardSearchResultImpl(const BufferView *view, const std::string &searchwildcard, size_t searchsize): SearchObjectImpl(view, searchsize), m_searchwildcard(searchwildcard) { }
WildcardSearchResult WildcardSearchResultImpl::next() const { return m_view->wildcard(m_searchwildcard, m_position + m_searchsize); }

const std::string BufferViewImpl::WILDCARD_BYTE = "??";

BufferViewImpl::BufferViewImpl(BufferView *q): m_pimpl_q(q) { }
size_t BufferViewImpl::patternLength(const std::string &pattern) const { return pattern.size() / 2;  }

std::pair<u8, u8> BufferViewImpl::patternRange(std::string &pattern, size_t &startoffset, size_t &endoffset, size_t &beginoffset) const
{
    std::pair<u8, u8> bp;

    for(size_t i = 0; i < pattern.size() - 2; i += 2)
    {
        std::string hexb = pattern.substr(i, 2);

        if(hexb == BufferViewImpl::WILDCARD_BYTE)
            continue;

        pattern = pattern.substr(i);     // Trim leading wildcards
        Utils::byte(hexb, &bp.first);
        beginoffset = i / 2;
        startoffset += i / 2;
        break;
    }

    for(s64 i = pattern.size() - 2; i >= 0; i -= 2)
    {
        std::string hexb = pattern.substr(i, 2);

        if(hexb == BufferViewImpl::WILDCARD_BYTE)
            continue;

        pattern = pattern.substr(0, i); // Trim trailing wildcards
        Utils::byte(hexb, &bp.second);
        endoffset = startoffset + (i / 2);
        break;
    }

    return bp;
}

bool BufferViewImpl::comparePattern(const std::string &pattern, const u8 *pdata) const
{
    const u8* pcurr = pdata;

    for(size_t i = 0; i < pattern.size() - 2; i += 2, pcurr++)
    {
        std::string hexb = pattern.substr(i, 2);

        if(hexb == BufferViewImpl::WILDCARD_BYTE)
            continue;

        u8 b = 0;

        if(!Utils::byte(hexb, &b) || (b != *pcurr))
            return false;
    }

    return true;
}

bool BufferViewImpl::preparePattern(std::string &pattern) const
{
    PIMPL_Q(const BufferView);

    if(q->eob() || pattern.empty())
        return false;

    pattern.erase(std::remove_if(pattern.begin(), pattern.end(), ::isspace), pattern.end());

    if((pattern.size() % 2) || (this->patternLength(pattern) > q->size()))
        return false;

    size_t wccount = 0;

    for(size_t i = 0; i < pattern.size() - 2; i += 2)
    {
        std::string hexb = pattern.substr(i, 2);

        if(hexb == BufferViewImpl::WILDCARD_BYTE)
        {
            wccount++;
            continue;
        }

        if(!std::isxdigit(hexb.front()) || !std::isxdigit(hexb.back()))
            return false;
    }

    return wccount < pattern.size();
}

u8 *BufferViewImpl::endData() const
{
    PIMPL_Q(const BufferView);
    return q->data() ? (q->data() + q->size()) : nullptr;
}

WildcardSearchResult BufferViewImpl::wildcard(std::string pattern, size_t startoffset) const
{
    PIMPL_Q(const BufferView);

    if(!this->preparePattern(pattern))
        return WildcardSearchResult();

    size_t beginoffset = 0, endoffset = startoffset, searchsize = this->patternLength(pattern);
    auto bp = this->patternRange(pattern, startoffset, endoffset, beginoffset);

    WildcardSearchResult r(q, pattern, searchsize);
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
            r.pimpl_p()->m_result = pdata;
            r.pimpl_p()->m_position = (pdata - q->data()) - beginoffset;
            break;
        }

        pdata++;
    }

    return r;
}

SearchResult BufferViewImpl::find(const u8 *searchdata, size_t searchsize, size_t startoffset) const
{
    PIMPL_Q(const BufferView);

    if(q->eob() || !searchdata || !searchsize || (searchsize > q->size()))
        return SearchResult();

    SearchResult r(q, searchdata, searchsize);
    const u8* pdata = q->data() + startoffset;

    while((pdata + searchsize) < this->endData())
    {
        if(!std::equal(pdata, pdata + searchsize, searchdata))
        {
            pdata++;
            continue;
        }

        r.pimpl_p()->m_result = pdata;
        r.pimpl_p()->m_position = pdata - q->data();
        break;
    }

    return r;
}

} // namespace REDasm
