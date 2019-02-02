#include "memorybuffer.h"
#include <algorithm>
#include <fstream>

namespace REDasm {
namespace Buffer {

MemoryBuffer::MemoryBuffer(): m_data(NULL), m_size(0) { }
MemoryBuffer::MemoryBuffer(u64 size): m_size(size) { m_data = reinterpret_cast<u8*>(std::malloc(size)); }

MemoryBuffer::MemoryBuffer(u64 size, u8 val)
{
    m_data = reinterpret_cast<u8*>(std::malloc(size));
    std::fill_n(m_data, size, val);
}

MemoryBuffer::MemoryBuffer(MemoryBuffer&& mb)
{
    this->swap(mb);

    mb.m_data = NULL;
    mb.m_size = 0;
}

MemoryBuffer::~MemoryBuffer()
{
    if(m_data)
        std::free(m_data);

    m_data = NULL;
    m_size = 0;
}

u8 *MemoryBuffer::data() const { return m_data; }
u64 MemoryBuffer::size() const { return m_size; }

void MemoryBuffer::resize(u64 size)
{
    m_data = reinterpret_cast<u8*>(std::realloc(m_data, size));

    if(!m_data)
        throw std::bad_alloc();

    m_size = size;
}

void MemoryBuffer::swap(MemoryBuffer &mb)
{
    std::swap(m_data, mb.m_data);
    std::swap(m_size, mb.m_size);
}

MemoryBuffer *MemoryBuffer::fromFile(const std::string &file)
{
    std::ifstream ifs(file, std::ios::in | std::ios::binary | std::ios::ate);

    if(!ifs.is_open())
        return NULL;

    size_t size = ifs.tellg();

    if(!size)
        return NULL;

    ifs.seekg(0, std::ios::beg);

    MemoryBuffer* b = new MemoryBuffer(size);
    ifs.read(reinterpret_cast<char*>(b->m_data), size);
    ifs.close();
    return b;
}

} // namespace Buffer
} // namespace REDasm
