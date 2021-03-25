#include "surface.h"
#include <rdcore/surface/surface.h>

RDSurface* RDSurface_Create(RDContext* ctx, rd_flag flags, uintptr_t userdata) { return CPTR(RDSurface, new Surface(CPTR(Context, ctx), flags, userdata)); }
const RDSurfacePos* RDSurface_GetPosition(const RDSurface* sf) { return CPTR(const Surface, sf)->cursor()->position(); }
const RDSurfacePos* RDSurface_GetSelection(const RDSurface* sf) { return CPTR(const Surface, sf)->cursor()->selection(); }
const RDSurfacePos* RDSurface_GetStartSelection(const RDSurface* sf) { return CPTR(const Surface, sf)->cursor()->startSelection(); }
const RDSurfacePos* RDSurface_GetEndSelection(const RDSurface* sf) { return CPTR(const Surface, sf)->cursor()->endSelection(); }
rd_address RDSurface_GetFirstAddress(const RDSurface* sf) { return CPTR(const Surface, sf)->firstAddress(); }
rd_address RDSurface_GetLastAddress(const RDSurface* sf) { return CPTR(const Surface, sf)->lastAddress(); }
rd_address RDSurface_GetCurrentAddress(const RDSurface* sf) { return CPTR(const Surface, sf)->currentAddress(); }
rd_address RDSurface_GetAddress(const RDSurface* sf, int row) { return CPTR(const Surface, sf)->getAddress(row); }
const char* RDSurface_GetSelectedText(const RDSurface* sf) { return CPTR(const Surface, sf)->selectedText().c_str(); }
uintptr_t RDSurface_GetUserData(const RDSurface* sf) { return CPTR(const Surface, sf)->userData(); }
size_t RDSurface_GetPath(const RDSurface* sf, const RDPathItem** path) { return CPTR(const Surface, sf)->getPath(path); }
int RDSurface_GetRow(const RDSurface* sf, int row, const RDSurfaceCell** cells) { return CPTR(const Surface, sf)->row(row, cells); }
int RDSurface_IndexOf(const RDSurface* sf, rd_address address) { return CPTR(const Surface, sf)->indexOf(address); }
int RDSurface_LastIndexOf(const RDSurface* sf, rd_address address) { return CPTR(const Surface, sf)->lastIndexOf(address); }
bool RDSurface_Contains(const RDSurface* sf, rd_address address) { return CPTR(const Surface, sf)->contains(address); }
bool RDSurface_GetLabelAt(const RDSurface* sf, int row, int col, rd_address* address) { return CPTR(const Surface, sf)->labelAt(row, col, address); }
bool RDSurface_Seek(RDSurface* sf, rd_address address) { return CPTR(Surface, sf)->goTo(address, false); }
bool RDSurface_GoTo(RDSurface* sf, rd_address address) { return CPTR(Surface, sf)->goTo(address, true); }
bool RDSurface_HasSelection(const RDSurface* sf) { return CPTR(const Surface, sf)->cursor()->hasSelection(); }
bool RDSurface_CanGoBack(const RDSurface* sf) { return CPTR(const Surface, sf)->cursor()->canGoBack(); }
bool RDSurface_CanGoForward(const RDSurface* sf) { return CPTR(const Surface, sf)->cursor()->canGoForward(); }
void RDSurface_GetScrollRange(const RDSurface* sf, rd_address* start, rd_address* end) { CPTR(const Surface, sf)->getScrollRange(start, end); }
void RDSurface_GetSize(const RDSurface* sf, int* rows, int* cols) { return CPTR(const Surface, sf)->getSize(rows, cols); }
void RDSurface_Update(RDSurface* sf) { CPTR(Surface, sf)->update(); }
void RDSurface_Scroll(RDSurface* sf, rd_address address, int ncols) { CPTR(Surface, sf)->scroll(address, ncols); }
void RDSurface_Resize(RDSurface* sf, int rows, int cols) { CPTR(Surface, sf)->resize(rows, cols); }
void RDSurface_MoveTo(RDSurface* sf, int row, int col) { CPTR(Surface, sf)->moveTo(row, col); }
void RDSurface_Select(RDSurface* sf, int row, int col) { CPTR(Surface, sf)->select(row, col); }
void RDSurface_SelectAt(RDSurface* sf, int row, int col) { CPTR(Surface, sf)->selectAt(row, col); }
void RDSurface_GoBack(RDSurface* sf) { return CPTR(Surface, sf)->cursor()->goBack(); }
void RDSurface_GoForward(RDSurface* sf) { return CPTR(Surface, sf)->cursor()->goForward(); }
void RDSurface_Activate(RDSurface* sf) { CPTR(Surface, sf)->activate(); }
void RDSurface_Deactivate(RDSurface* sf) { CPTR(Surface, sf)->deactivate(); }
void RDSurface_LinkTo(RDSurface* sf, RDSurface* s) { CPTR(Surface, sf)->linkTo(CPTR(Surface, s)); }
void RDSurface_Unlink(RDSurface* sf) { CPTR(Surface, sf)->unlink(); }

const char* RDSurface_GetCurrentWord(const RDSurface* sf)
{
    auto* cw = CPTR(const Surface, sf)->currentWord();
    return cw ? cw->c_str() : nullptr;
}

const char* RDSurface_GetWordAt(const RDSurface* sf, int row, int col)
{
    auto* w = CPTR(const Surface, sf)->wordAt(row, col);
    return w ? w->c_str() : nullptr;
}

const char* RDSurface_GetCurrentLabel(const RDSurface* sf, rd_address* address)
{
    static std::string s;

    auto* ps = CPTR(const Surface, sf)->currentLabel(address);
    if(!ps) return nullptr;

    s = *ps;
    return s.c_str();
}
