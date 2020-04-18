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
        BufferView* view(offset_t offset, size_t size);

    public:
        static MemoryBuffer* fromFile(const char* filename);

    private:
        std::vector<u8> m_data;
};

