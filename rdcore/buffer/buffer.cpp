#include "buffer.h"
#include <algorithm>
#include <fstream>

MemoryBuffer::MemoryBuffer(size_t size) { this->resize(size); }
void MemoryBuffer::resize(size_t size) { m_data.resize(size); }

BufferView* MemoryBuffer::view(offset_t offset, size_t size)
{
    if(offset >= this->size()) return nullptr;
    size = std::min(size, this->size());
    return new BufferView(this->data() + offset, size);
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
