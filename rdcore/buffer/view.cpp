#include "view.h"
#include <rdcore/support/utils.h>

bool BufferView::empty(const RDBufferView* view) { return !view->data || !view->size; }

void BufferView::advance(RDBufferView* view, size_t offset)
{
    if(offset > view->size) offset = view->size; // Avoid buffer overflow
    view->data += offset;
    view->size -= offset;
}

u16 BufferView::crc16(const RDBufferView* view, rd_offset offset, size_t size) { return Utils::crc16(view->data, view->size, offset, size); }
u32 BufferView::crc32(const RDBufferView* view, rd_offset offset, size_t size) { return Utils::crc32(view->data, view->size, offset, size); }

u8* BufferView::find(const RDBufferView* view, const u8* finddata, size_t findsize)
{
    rd_offset offset = Utils::findIn(view->data, view->size, finddata, findsize);
    return offset == RD_NVAL ? view->data + offset : nullptr;
}

u8* BufferView::findNext(RDBufferView* view, const u8* finddata, size_t findsize)
{
   if(!view->size) return nullptr;

   rd_offset offset = Utils::findIn(view->data, view->size, finddata, findsize);
   if(offset == RD_NVAL) return nullptr;

   u8* res = view->data + offset;
   BufferView::advance(view, offset + findsize);
   return res;
}
