#include "surface.h"
#include <rdcore/renderer/surface.h>

RDSurface* RDSurface_Create(RDContext* ctx, rd_flag flags) { return CPTR(RDSurface, new Surface(CPTR(Context, ctx), flags)); }
const RDCursorPos* RDSurface_GetPosition(const RDSurface* s) { return CPTR(const Surface, s)->position(); }
const RDCursorPos* RDSurface_GetSelection(const RDSurface* s) { return CPTR(const Surface, s)->selection(); }
const RDCursorPos* RDSurface_GetStartSelection(const RDSurface* s) { return CPTR(const Surface, s)->startSelection(); }
const RDCursorPos* RDSurface_GetEndSelection(const RDSurface* s) { return CPTR(const Surface, s)->endSelection(); }
int RDSurface_GetRow(const RDSurface* sf, int row, RDSurfaceCell* cells) { return CPTR(const Surface, sf)->row(row, cells); }
bool RDSurface_GetSelectedSymbol(const RDSurface* sf, RDSymbol* symbol) { return CPTR(const Surface, sf)->getSelectedSymbol(symbol); }
bool RDSurface_GoTo(RDSurface* sf, const RDDocumentItem* item) { return CPTR(Surface, sf)->goTo(item); }
bool RDSurface_GoToAddress(RDSurface* sf, rd_address address) { return CPTR(Surface, sf)->goToAddress(address); }
void RDSurface_GetSize(const RDSurface* sf, int* rows, int* cols) { return CPTR(const Surface, sf)->getSize(rows, cols); }
void RDSurface_Scroll(RDSurface* sf, int nrows, int ncols) { CPTR(Surface, sf)->scroll(nrows, ncols); }
void RDSurface_Resize(RDSurface* sf, int rows, int cols) { CPTR(Surface, sf)->resize(rows, cols); }
void RDSurface_MoveTo(RDSurface* sf, int row, int col) { CPTR(Surface, sf)->moveTo(row, col); }
void RDSurface_Select(RDSurface* sf, int row, int col) { CPTR(Surface, sf)->select(row, col); }
void RDSurface_GoBack(RDSurface* sf) { return CPTR(Surface, sf)->goBack(); }
void RDSurface_GoForward(RDSurface* sf) { return CPTR(Surface, sf)->goForward(); }
void RDSurface_EnableCursor(RDSurface* sf) { CPTR(Surface, sf)->enable(); }
void RDSurface_DisableCursor(RDSurface* sf) { CPTR(Surface, sf)->disable(); }
bool RDSurface_HasSelection(const RDSurface* sf) { return CPTR(const Surface, sf)->hasSelection(); }
bool RDSurface_CanGoBack(const RDSurface* sf) { return CPTR(const Surface, sf)->canGoBack(); }
bool RDSurface_CanGoForward(const RDSurface* sf) { return CPTR(const Surface, sf)->canGoForward(); }
bool RDSurface_GetCurrentItem(const RDSurface* sf, RDDocumentItem* item) { return CPTR(const Surface, sf)->getCurrentItem(item); }

const char* RDSurface_GetCurrentWord(const RDSurface* sf)
{
    auto* cw = CPTR(const Surface, sf)->currentWord();
    return cw ? cw->c_str() : nullptr;
}
