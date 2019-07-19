#include "memorybuffer.h"
#include <impl/buffer/memorybuffer_impl.h>
#include <redasm/support/compression.h>
#include <algorithm>
#include <fstream>

namespace REDasm {

MemoryBuffer::MemoryBuffer(): m_pimpl_p(new MemoryBufferImpl()) { }

MemoryBuffer::MemoryBuffer(size_t size): m_pimpl_p(new MemoryBufferImpl())
{
    PIMPL_P(MemoryBuffer);

    p->m_size = size;
    p->m_data = reinterpret_cast<u8*>(std::malloc(size));
}

MemoryBuffer::MemoryBuffer(size_t size, u8 val): m_pimpl_p(new MemoryBufferImpl())
{
    PIMPL_P(MemoryBuffer);

    p->m_data = reinterpret_cast<u8*>(std::malloc(size));
    std::fill_n(p->m_data, size, val);
}

MemoryBuffer::MemoryBuffer(MemoryBuffer&& mb)
{
    this->swap(mb);

    mb.pimpl_p()->m_data = nullptr;
    mb.pimpl_p()->m_size = 0;
}

MemoryBuffer::~MemoryBuffer()
{
    PIMPL_P(MemoryBuffer);

    if(p->m_data)
        std::free(p->m_data);

    p->m_data = nullptr;
    p->m_size = 0;
}

u8 *MemoryBuffer::data() const { PIMPL_P(const MemoryBuffer); return p->m_data; }
size_t MemoryBuffer::size() const { PIMPL_P(const MemoryBuffer); return p->m_size; }

void MemoryBuffer::resize(size_t size)
{
    PIMPL_P(MemoryBuffer);
    p->m_data = reinterpret_cast<u8*>(std::realloc(p->m_data, size));

    if(!p->m_data)
        throw std::bad_alloc();

    p->m_size = size;
}

void MemoryBuffer::swap(MemoryBuffer &mb)
{
    PIMPL_P(MemoryBuffer);

    std::swap(p->m_data, mb.pimpl_p()->m_data);
    std::swap(p->m_size, mb.pimpl_p()->m_size);
}

void MemoryBuffer::save(cereal::BinaryOutputArchive &a) const { PIMPL_P(const MemoryBuffer); p->save(a); }
void MemoryBuffer::load(cereal::BinaryInputArchive &a) { PIMPL_P(MemoryBuffer); p->load(a); }

MemoryBuffer *MemoryBuffer::fromFile(const String &file)
{
    std::ifstream ifs(file.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

    if(!ifs.is_open())
        return nullptr;

    size_t size = ifs.tellg();

    if(!size)
        return nullptr;

    ifs.seekg(0, std::ios::beg);

    MemoryBuffer* b = new MemoryBuffer(size);
    ifs.read(reinterpret_cast<char*>(b->pimpl_p()->m_data), size);
    ifs.close();
    return b;
}

MemoryBuffer *MemoryBuffer::from(const u8 *data, size_t size)
{
    MemoryBuffer* b = new MemoryBuffer(size);
    std::copy(data, data + size, b->data());
    return b;
}

} // namespace REDasm
