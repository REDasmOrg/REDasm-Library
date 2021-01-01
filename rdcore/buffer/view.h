#pragma once

#include <rdapi/buffer.h>

class BufferView
{
    public:
        BufferView() = delete;
        static bool empty(const RDBufferView* view);
        static void advance(RDBufferView* view, size_t offset);
        static u16 crc16(const RDBufferView* view, rd_offset offset, size_t size);
        static u32 crc32(const RDBufferView* view, rd_offset offset, size_t size);
        static u8* find(const RDBufferView* view, const u8* finddata, size_t findsize);
        static u8* findNext(RDBufferView* view, const u8* finddata, size_t findsize);
        static u8* findPattern(const RDBufferView* view, const char* pattern);
        static u8* findPatternNext(RDBufferView* view, const char* pattern);
};

