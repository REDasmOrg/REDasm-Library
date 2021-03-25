#pragma once

#include <vector>
#include "abstract.h"
#include "view.h"

class MemoryBuffer: public AbstractBuffer
{
    public:
        MemoryBuffer(size_t size);
        size_t size() const override;
        u8* data() override;
        void resize(size_t size);
        bool view(rd_offset offset, size_t size, RDBufferView* view) const;

    public:
        using AbstractBuffer::data;
        static MemoryBuffer* fromFile(const char* filename);

    private:
        std::vector<u8> m_data;
};

typedef std::shared_ptr<MemoryBuffer> MemoryBufferPtr;
