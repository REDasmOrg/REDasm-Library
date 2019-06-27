#pragma once

#include "abstractbuffer.h"
#include "../pimpl.h"

namespace REDasm {

class MemoryBufferImpl;

class LIBREDASM_API MemoryBuffer: public AbstractBuffer
{
    REDASM_OBJECT(MemoryBuffer)
    PIMPL_DECLARE_P(MemoryBuffer)
    PIMPL_DECLARE_PRIVATE(MemoryBuffer)

    public:
        MemoryBuffer();
        MemoryBuffer(size_t size);
        MemoryBuffer(size_t size, u8 val);
        MemoryBuffer(const MemoryBuffer&) = delete;
        MemoryBuffer(MemoryBuffer&& mb);
        ~MemoryBuffer();
        u8* data() const override;
        size_t size() const override;
        void resize(size_t size) override;
        void swap(MemoryBuffer& mb);

    public:
        static MemoryBuffer* fromFile(const String& file);
};

} // namespace REDasm
