#include "bufferview_impl.h"
#include <redasm/support/utils.h>

namespace REDasm {

BufferViewImpl::BufferViewImpl(BufferView *q): m_pimpl_q(q) { }
size_t BufferViewImpl::patternLength(const std::string &pattern) const { return pattern.size() / 2;  }

std::pair<u8, u8> BufferViewImpl::patternRange(std::string &pattern, size_t &startoffset, size_t &endoffset, size_t &beginoffset) const
{
    std::pair<u8, u8> bp;

    for(size_t i = 0; i < pattern.size() - 2; i += 2)
    {
        std::string hexb = pattern.substr(i, 2);

        if(hexb == BufferView::WILDCARD_BYTE)
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

        if(hexb == BufferView::WILDCARD_BYTE)
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

        if(hexb == BufferView::WILDCARD_BYTE)
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

        if(hexb == BufferView::WILDCARD_BYTE)
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

} // namespace REDasm
