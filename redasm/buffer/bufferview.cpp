#include "bufferview.h"
#include <impl/buffer/bufferview_impl.h>
#include <redasm/support/utils.h>
#include <stdexcept>
#include <cctype>

namespace REDasm {

const std::string BufferView::WILDCARD_BYTE = "??";

BufferView::BufferView(): m_pimpl_p(new BufferViewImpl(this)) { }

BufferView::BufferView(const AbstractBuffer *buffer, size_t offset, size_t size): m_pimpl_p{new BufferViewImpl(this)}
{
    PIMPL_P(BufferView);

    p->m_buffer = buffer;
    p->m_offset = offset;
    p->m_size = size;
}

BufferView BufferView::view(size_t offset, size_t size) const
{
    PIMPL_P(const BufferView);

    if(!size)
        size = this->size() - offset;

    if(offset >= this->size())
        throw std::out_of_range("Buffer:slice(): offset out of bounds");

    return BufferView(p->m_buffer, p->m_offset + offset, size);
}

u8 BufferView::operator[](size_t idx) const
{
    PIMPL_P(const BufferView);

    if(idx >= p->m_size)
        throw std::out_of_range("Buffer::operator[]: index is out of range");

    return this->data()[idx];
}

u8 &BufferView::operator[](size_t idx)
{
    PIMPL_P(BufferView);

    if(idx >= p->m_size)
        throw std::out_of_range("Buffer::operator[]: index is out of range");

    return this->data()[idx];
}

BufferView &BufferView::operator++()
{
    PIMPL_P(BufferView);

    if(!p->m_size)
        throw std::out_of_range("Buffer:operator++(): reached end of buffer");

     p->m_offset++;
     p->m_size--;
     return *this;
}

BufferView BufferView::operator ++(int)
{
    PIMPL_P(BufferView);

    if(!p->m_size)
        throw std::out_of_range("Buffer:operator++(int): reached end of buffer");

     BufferView ret(p->m_buffer, p->m_offset, p->m_size);
     p->m_offset++;
     p->m_size--;
     return ret;
}

void BufferView::copyTo(AbstractBuffer *buffer)
{
    PIMPL_P(BufferView);

    if(buffer->size() < p->m_size)
        buffer->resize(p->m_size);

    std::copy_n(this->data(), p->m_size, buffer->data());
}

std::string BufferView::toString() const { return std::string(reinterpret_cast<const char*>(this->data()), this->size()); }
void BufferView::resize(size_t size) { PIMPL_P(BufferView); p->m_size = std::min(size, p->m_buffer->size()); }
BufferView::byte_iterator BufferView::begin() const { return byte_iterator(this->data()); }
BufferView::byte_iterator BufferView::end() const { PIMPL_P(const BufferView); return byte_iterator(p->endData()); }
u8 *BufferView::data() const { PIMPL_P(const BufferView); return p->m_buffer->data() + p->m_offset; }
u8 *BufferView::endData() const { PIMPL_P(const BufferView); return this->data() ? (this->data() + this->size()) : nullptr; }
const AbstractBuffer *BufferView::buffer() const { PIMPL_P(const BufferView); return p->m_buffer; }
bool BufferView::inRange(size_t offset) const { PIMPL_P(const BufferView); return (offset >= p->m_offset) && (offset < (p->m_offset + p->m_size)); }
bool BufferView::eob() const { PIMPL_P(const BufferView); return !p->m_buffer || !this->data() || !p->m_size; }
offset_t BufferView::offset() const { PIMPL_P(const BufferView); return p->m_offset; }
size_t BufferView::size() const { PIMPL_P(const BufferView); return p->m_size; }
u8 BufferView::operator *() const { return *this->data(); }

BufferView BufferView::operator +(size_t offset) const
{
    if(offset > this->size())
        throw std::out_of_range("Buffer:operator+=(): offset > size");

    return BufferView(this->buffer(), this->offset() + offset, this->size() - offset);
}

BufferView &BufferView::operator +=(size_t offset)
{
    if(offset > this->size())
        throw std::out_of_range("Buffer:operator+=(): offset > size");

    PIMPL_P(BufferView);
    p->m_offset += offset;
    p->m_size -= offset;
    return *this;
}


} // namespace REDasm