#include "buffer.h"
#include <rdcore/buffer/buffer.h>
#include <rdcore/buffer/view.h>

RDBuffer* RDBuffer_Create(size_t size) { return CPTR(RDBuffer, new MemoryBuffer(size)); }
RDBuffer* RDBuffer_CreateFromFile(const char* filename) { return CPTR(RDBuffer, MemoryBuffer::fromFile(filename)); }
u8* RDBuffer_Data(RDBuffer* buffer) { return CPTR(MemoryBuffer, buffer)->data(); }
size_t RDBuffer_Size(const RDBuffer* buffer) { return CPTR(const MemoryBuffer, buffer)->size(); }
rd_offset RDBuffer_Find(const RDBuffer* buffer, const u8* data, size_t size) { return CPTR(const MemoryBuffer, buffer)->find(data, size); }
void RDBuffer_Fill(RDBuffer* buffer, u8 val) { CPTR(MemoryBuffer, buffer)->fill(val); }
void RDBuffer_Resize(RDBuffer* buffer, size_t size) { CPTR(MemoryBuffer, buffer)->resize(size); }
u16 RDBuffer_CRC16(const RDBuffer* buffer, rd_offset offset, size_t size) { return CPTR(const MemoryBuffer, buffer)->crc16(offset, size); }
u32 RDBuffer_CRC32(const RDBuffer* buffer, rd_offset offset, size_t size) { return CPTR(const MemoryBuffer, buffer)->crc32(offset, size); }

u8* RDBufferView_Find(const RDBufferView* view, const u8* data, size_t size) { return BufferView::find(view, data, size); }
u8* RDBufferView_FindNext(RDBufferView* view, const u8* data, size_t size) { return BufferView::findNext(view, data, size); }
u16 RDBufferView_CRC16(const RDBufferView* view, rd_offset offset, size_t size) { return BufferView::crc16(view, offset, size); }
u32 RDBufferView_CRC32(const RDBufferView* view, rd_offset offset, size_t size) { return BufferView::crc32(view, offset, size); }
void RDBufferView_Advance(RDBufferView* view, size_t offset) { BufferView::advance(view, offset); }
