#include "abstractbuffer.h"
#include "bufferview.h"

namespace REDasm {

u8 &AbstractBuffer::at(size_t idx)
{
    if(idx >= this->size())
        throw std::out_of_range("AbstractBuffer::operator[]: Index out of range");

    return this->data()[idx];
}

u8 AbstractBuffer::at(size_t idx) const
{
    if(idx >= this->size())
        throw std::out_of_range("AbstractBuffer::operator[]: Index out of range");

    return this->data()[idx];
}

bool AbstractBuffer::empty() const { return !this->data() || !this->size(); }
BufferView AbstractBuffer::view() const { return this->view(0); }

BufferView AbstractBuffer::view(size_t offset, size_t size) const
{
    if(!size)
        size = this->size();

    return BufferView(this, offset, size);
}

u8& AbstractBuffer::operator[](size_t idx) { return this->at(idx); }
u8 AbstractBuffer::operator[](size_t idx) const { return this->at(idx); }

} // namespace REDasm
