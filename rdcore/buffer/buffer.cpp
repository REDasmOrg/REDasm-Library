#include "buffer.h"
#include <algorithm>
#include <fstream>

MemoryBuffer::MemoryBuffer(size_t size) { this->resize(size); }
void MemoryBuffer::resize(size_t size) { m_data.resize(size); }

bool MemoryBuffer::view(rd_offset offset, size_t size, RDBufferView* view) const
{
    if(offset >= this->size()) { };
    size = std::min(size, this->size() - offset);
    if(view) *view = { const_cast<MemoryBuffer*>(this)->data() + offset, size };
    return !BufferView::empty(view);
}

size_t MemoryBuffer::size() const { return m_data.size(); }
u8* MemoryBuffer::data() { return m_data.data(); }

MemoryBuffer* MemoryBuffer::fromFile(const char* filename)
{
    std::ifstream ifs(filename, std::ios::in | std::ios::binary | std::ios::ate);
    if(!ifs.is_open()) return nullptr;

    auto size = ifs.tellg();
    if(!size) return nullptr;

    ifs.seekg(0, std::ios::beg);

    MemoryBuffer* b = new MemoryBuffer(static_cast<size_t>(size));
    ifs.read(reinterpret_cast<char*>(b->m_data.data()), size);
    return b;
}
