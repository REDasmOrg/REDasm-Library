#pragma once

#include <redasm/buffer/memorybuffer.h>
#include <redasm/types/base.h>
#include <redasm/pimpl.h>

namespace REDasm {

class MemoryBufferImpl
{
    PIMPL_DECLARE_Q(MemoryBuffer)
    PIMPL_DECLARE_PUBLIC(MemoryBuffer)

    public:
        MemoryBufferImpl();
        void save(cereal::BinaryOutputArchive& a) const;
        void load(cereal::BinaryInputArchive& a);

    private:
        u8* m_data;
        size_t m_size;
};

} // namespace REDasm
