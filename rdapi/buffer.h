#pragma once

#include "types.h"
#include "macros.h"

RD_HANDLE(RDBuffer);

RD_API_EXPORT RDBuffer* RDBuffer_Create(size_t size);
RD_API_EXPORT RDBuffer* RDBuffer_CreateFromFile(const char* filename);
RD_API_EXPORT u8* RDBuffer_Data(RDBuffer* buffer);
RD_API_EXPORT size_t RDBuffer_Size(const RDBuffer* buffer);
RD_API_EXPORT rd_offset RDBuffer_Find(const RDBuffer* buffer, const u8* data, size_t size);
RD_API_EXPORT void RDBuffer_Fill(RDBuffer* buffer, u8 val);
RD_API_EXPORT void RDBuffer_Resize(RDBuffer* buffer, size_t size);
RD_API_EXPORT u16 RDBuffer_CRC16(const RDBuffer* buffer, rd_offset offset, size_t size);
RD_API_EXPORT u32 RDBuffer_CRC32(const RDBuffer* buffer, rd_offset offset, size_t size);

typedef struct RDBufferView {
    u8* data;
    size_t size;
} RDBufferView;

RD_API_EXPORT rd_offset RDBufferView_Find(const RDBufferView* view, const u8* data, size_t size);
RD_API_EXPORT u16 RDBufferView_CRC16(const RDBufferView* view, rd_offset offset, size_t size);
RD_API_EXPORT u32 RDBufferView_CRC32(const RDBufferView* view, rd_offset offset, size_t size);
