#include "memorybuffer_impl.h"
#include "../libs/cereal/archives/binary.hpp"

namespace REDasm {

MemoryBufferImpl::MemoryBufferImpl(): m_data(nullptr), m_size(0) { }

void MemoryBufferImpl::save(cereal::BinaryOutputArchive &a) const
{
    a(m_size);
    a.saveBinary(m_data, m_size);
}

void MemoryBufferImpl::load(cereal::BinaryInputArchive &a)
{
    a(m_size);
    m_data = reinterpret_cast<u8*>(std::malloc(m_size));
    a.loadBinary(m_data, m_size);
}

} // namespace REDasm
