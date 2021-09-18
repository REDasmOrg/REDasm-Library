#include "view.h"
#include "../support/utils.h"
#include "../support/hash.h"

bool BufferView::empty(const RDBufferView* view) { return !view->data || !view->size; }

void BufferView::move(RDBufferView* view, s64 offset)
{
    if((offset > 0) && static_cast<size_t>(offset) > view->size) offset = view->size; // Avoid buffer overflow
    view->data += offset;
    view->size -= offset;
}

u16 BufferView::crc16(const RDBufferView* view, rd_offset offset, size_t size) { return Hash::crc16(view->data, view->size, offset, size); }
u32 BufferView::crc32(const RDBufferView* view, rd_offset offset, size_t size) { return Hash::crc32(view->data, view->size, offset, size); }

u8* BufferView::find(const RDBufferView* view, const u8* finddata, size_t findsize)
{
    rd_offset offset = Utils::findIn(view->data, view->size, finddata, findsize);
    return offset == RD_NVAL ? view->data + offset : nullptr;
}

u8* BufferView::findNext(RDBufferView* view, const u8* finddata, size_t findsize)
{
   if(!view->size) return nullptr;

   rd_offset offset = Utils::findIn(view->data, view->size, finddata, findsize);

   if(offset == RD_NVAL)
   {
       BufferView::move(view, 1);
       return nullptr;
   }

   u8* res = view->data + offset;
   BufferView::move(view, offset + findsize);
   return res;
}

u8* BufferView::findPattern(const RDBufferView* view, const char* pattern)
{
    if(!pattern) return nullptr;
    rd_offset offset = Utils::findPattern(view->data, view->size, pattern);
    return offset == RD_NVAL ? view->data + offset : nullptr;
}

u8* BufferView::findPatternNext(RDBufferView* view, const char* pattern)
{
    if(!pattern) return nullptr;
    size_t patternlen = 0;
    rd_offset offset = Utils::findPattern(view->data, view->size, pattern, &patternlen);

    if(offset == RD_NVAL)
    {
        BufferView::move(view, 1);
        return nullptr;
    }

    u8* res = view->data + offset;
    BufferView::move(view, offset + patternlen);
    return res;
}
