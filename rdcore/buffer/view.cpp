#include "view.h"

BufferView::BufferView(): m_data(nullptr), m_size(0) { }
BufferView::BufferView(u8* data, size_t size): m_data(data), m_size(size) { }
size_t BufferView::size() const { return m_size; }
u8* BufferView::data() { return m_data; }

BufferView& BufferView::advance(size_t offset)
{
    m_data += offset;
    m_size -= offset;
    return *this;
}

void BufferView::copyTo(BufferView* dest) const
{
    dest->m_data = m_data;
    dest->m_size = m_size;
}
