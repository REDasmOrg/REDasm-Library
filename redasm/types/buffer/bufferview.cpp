#include "bufferview.h"
#include "../../support/utils.h"

namespace REDasm {
namespace Buffer {

const std::string BufferView::WILDCARD_BYTE = "??";

BufferView::BufferView(): m_buffer(NULL), m_offset(0), m_size(0) { }
BufferView::BufferView(const AbstractBuffer *buffer, u64 offset, u64 size): m_buffer(buffer), m_offset(offset), m_size(size) { }

BufferView BufferView::view(size_t offset, size_t size) const
{
    if(!size)
        size = m_size - offset;

    if(offset >= m_size)
        throw std::out_of_range("Buffer:slice(): offset out of bounds");

    return BufferView(m_buffer, m_offset + offset, size);
}

u8 BufferView::operator[](size_t idx) const
{
    if(idx >= m_size)
        throw std::out_of_range("Buffer::operator[]: index is out of range");

    return this->data()[idx];
}

u8 &BufferView::operator[](size_t idx)
{
    if(idx >= m_size)
        throw std::out_of_range("Buffer::operator[]: index is out of range");

    return this->data()[idx];
}

BufferView &BufferView::operator++()
{
    if(!m_size)
        throw std::out_of_range("Buffer:operator++(): reached end of buffer");

     m_offset++;
     m_size--;
     return *this;
}

BufferView BufferView::operator ++(int)
{
    if(!m_size)
        throw std::out_of_range("Buffer:operator++(int): reached end of buffer");

     BufferView ret(m_buffer, m_offset, m_size);
     m_offset++;
     m_size--;
     return ret;
}

void BufferView::copyTo(AbstractBuffer *buffer)
{
    if(buffer->size() < m_size)
        buffer->resize(m_size);

    std::copy_n(this->data(), m_size, buffer->data());
}

void BufferView::resize(u64 size) { m_size = std::min(size, m_buffer->size()); }

std::pair<u8, u8> BufferView::patternRange(std::string &pattern, u64 &startoffset, u64 &endoffset, u64& beginoffset) const
{
    std::pair<u8, u8> bp;

    for(size_t i = 0; i < pattern.size() - 2; i += 2)
    {
        std::string hexb = pattern.substr(i, 2);

        if(hexb == BufferView::WILDCARD_BYTE)
            continue;

        pattern = pattern.substr(i);     // Trim leading wildcards
        bp.first = REDasm::byte(hexb);
        beginoffset = i / 2;
        startoffset += i / 2;
        break;
    }

    for(int i = pattern.size() - 2; i >= 0; i -= 2)
    {
        std::string hexb = pattern.substr(i, 2);

        if(hexb == BufferView::WILDCARD_BYTE)
            continue;

        pattern = pattern.substr(0, i); // Trim trailing wildcards
        bp.second = REDasm::byte(hexb);
        endoffset = startoffset + (i / 2);
        break;
    }

    return bp;
}

bool BufferView::comparePattern(const std::string &pattern, const u8 *pdata) const
{
    const u8* pcurr = pdata;

    for(size_t i = 0; i < pattern.size() - 2; i += 2, pcurr++)
    {
        std::string hexb = pattern.substr(i, 2);

        if(hexb == BufferView::WILDCARD_BYTE)
            continue;

        if(REDasm::byte(hexb) != *pcurr)
            return false;
    }

    return true;
}

bool BufferView::preparePattern(std::string &pattern) const
{
    if(this->eob() || pattern.empty())
        return false;

    pattern.erase(std::remove_if(pattern.begin(), pattern.end(), ::isspace), pattern.end());

    if((pattern.size() % 2) || (this->patternLength(pattern) > this->size()))
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

} // namespace Buffer
} // namespace REDasm
