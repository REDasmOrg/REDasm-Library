#pragma once

#include "abstract.h"

class BufferView: public AbstractBuffer
{
    public:
        BufferView();
        BufferView(u8* data, size_t size);
        size_t size() const override;
        u8* data() override;
        BufferView& advance(size_t offset);
        void copyTo(BufferView* dest) const;

    public:
        using AbstractBuffer::data;

    private:
        u8* m_data;
        size_t m_size;
};

