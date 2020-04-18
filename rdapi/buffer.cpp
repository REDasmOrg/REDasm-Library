#include "buffer.h"
#include <rdcore/buffer/buffer.h>
#include <rdcore/buffer/view.h>

RDBuffer* RDBuffer_Create(size_t size) { return CPTR(RDBuffer, new MemoryBuffer(size)); }
RDBuffer* RDBuffer_CreateFromFile(const char* filename) { return CPTR(RDBuffer, MemoryBuffer::fromFile(filename)); }
u8* RDBuffer_Data(RDBuffer* buffer) { return CPTR(MemoryBuffer, buffer)->data(); }
size_t RDBuffer_Size(const RDBuffer* buffer) { return CPTR(const MemoryBuffer, buffer)->size(); }
void RDBuffer_Fill(RDBuffer* buffer, u8 val) { CPTR(MemoryBuffer, buffer)->fill(val); }
void RDBuffer_Resize(RDBuffer* buffer, size_t size) { CPTR(MemoryBuffer, buffer)->resize(size); }

u8* RDBufferView_Data(RDBufferView* view) { return CPTR(BufferView, view)->data(); }
size_t RDBufferView_Size(const RDBufferView* view) { return CPTR(const BufferView, view)->size(); }
