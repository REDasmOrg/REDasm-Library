#include "bufferview.h"

namespace REDasm {
namespace Buffer {

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

} // namespace Buffer
} // namespace REDasm
