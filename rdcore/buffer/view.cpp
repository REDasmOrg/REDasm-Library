#include "view.h"
#include <rdcore/support/utils.h>

bool BufferView::empty(const RDBufferView* view) { return !view->data || !view->size; }

void BufferView::advance(RDBufferView* view, size_t offset)
{
    view->data += offset;
    view->size -= offset;
}

u16 BufferView::crc16(const RDBufferView* view, rd_offset offset, size_t size) { return Utils::crc16(view->data, view->size, offset, size); }
u32 BufferView::crc32(const RDBufferView* view, rd_offset offset, size_t size) { return Utils::crc32(view->data, view->size, offset, size); }
rd_offset BufferView::find(const RDBufferView* view, const u8* finddata, size_t findsize) { return Utils::findIn(view->data, view->size, finddata, findsize);  }
