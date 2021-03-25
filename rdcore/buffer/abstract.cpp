#include "abstract.h"
#include "../support/error.h"
#include "../support/utils.h"
#include "../support/hash.h"
#include <algorithm>

u8 AbstractBuffer::at(size_t offset) const
{
    if(!this->data() || (offset >= this->size())) REDasmError("Offset out of range");
    return this->data()[offset];
}

const u8* AbstractBuffer::data() const { return const_cast<AbstractBuffer*>(this)->data(); }
const u8* AbstractBuffer::endData() const { return this->data() ? (this->data() + this->size()) : nullptr; }
rd_offset AbstractBuffer::find(const u8* data, size_t size) const { return Utils::findIn(this->data(), this->size(), data, size); }

bool AbstractBuffer::contains(const u8* ptr) const
{
    return (reinterpret_cast<const u8*>(ptr) >= this->data()) &&
           (reinterpret_cast<const u8*>(ptr) < this->endData());
}

bool AbstractBuffer::empty() const { return !this->data() || !this->size(); }

void AbstractBuffer::copyFrom(const AbstractBuffer* buffer, rd_offset offset, size_t size)
{
    if(size == RD_NVAL) size = this->size();
    std::copy_n(buffer->data() + offset, size, this->data());
}

void AbstractBuffer::fill(u8 val) { std::fill_n(this->data(), this->size(), val); }
AbstractBuffer::operator bool() const { return !this->empty(); }

u16 AbstractBuffer::crc16(rd_offset offset, size_t size) const { return Utils::crc16(this->data(), this->size(), offset, size); }
u32 AbstractBuffer::crc32(rd_offset offset, size_t size) const { return Utils::crc32(this->data(), this->size(), offset, size); }
