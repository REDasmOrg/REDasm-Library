#include "abstractbuffer.h"
#include "bufferview.h"

namespace REDasm {
namespace Buffer {

u8 &AbstractBuffer::at(u64 idx)
{
    if(idx >= this->size())
        throw std::out_of_range("AbstractBuffer::operator[]: Index out of range");

    return this->data()[idx];
}

u8 AbstractBuffer::at(u64 idx) const
{
    if(idx >= this->size())
        throw std::out_of_range("AbstractBuffer::operator[]: Index out of range");

    return this->data()[idx];
}

bool AbstractBuffer::empty() const { return !this->data() || !this->size(); }
BufferView AbstractBuffer::view() const { return this->view(0); }

BufferView AbstractBuffer::view(u64 offset, u64 size) const
{
    if(size == -1u)
        size = this->size();

    return BufferView(this, offset, size);
}

u8& AbstractBuffer::operator[](u64 idx) { return this->at(idx); }
u8 AbstractBuffer::operator[](u64 idx) const { return this->at(idx); }

} // namespace Buffer
} // namespace REDasm
