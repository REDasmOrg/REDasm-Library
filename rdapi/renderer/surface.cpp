#include "surface.h"
#include <rdcore/renderer/surface.h>

RDSurface* RDSurface_Create(RDContext* ctx, rd_flag flags) { return CPTR(RDSurface, new Surface(CPTR(Context, ctx), flags)); }
const RDSurfacePos* RDSurface_GetPosition(const RDSurface* sf) { return CPTR(const Surface, sf)->position(); }
const RDSurfacePos* RDSurface_GetSelection(const RDSurface* sf) { return CPTR(const Surface, sf)->selection(); }
const RDSurfacePos* RDSurface_GetStartSelection(const RDSurface* sf) { return CPTR(const Surface, sf)->startSelection(); }
const RDSurfacePos* RDSurface_GetEndSelection(const RDSurface* sf) { return CPTR(const Surface, sf)->endSelection(); }
const RDDocumentItem* RDSurface_GetFirstItem(const RDSurface* sf) { return CPTR(const Surface, sf)->firstItem(); }
const RDDocumentItem* RDSurface_GetLastItem(const RDSurface* sf) { return CPTR(const Surface, sf)->lastItem(); }
const char* RDSurface_GetSelectedText(const RDSurface* sf) { return CPTR(const Surface, sf)->selectedText().c_str(); }
size_t RDSurface_GetPath(const RDSurface* sf, const RDPathItem** path) { return CPTR(const Surface, sf)->getPath(path); }
int RDSurface_GetRow(const RDSurface* sf, int row, const RDSurfaceCell** cells) { return CPTR(const Surface, sf)->row(row, cells); }
int RDSurface_FindRow(const RDSurface* sf, const RDDocumentItem* item) { return CPTR(const Surface, sf)->findRow(item); }
bool RDSurface_GetItem(const RDSurface* sf, int row, RDDocumentItem* item) { return CPTR(const Surface, sf)->getItem(row, item); }
bool RDSurface_Contains(const RDSurface* sf, const RDDocumentItem* item) { return CPTR(const Surface, sf)->contains(item); }
bool RDSurface_GetCurrentSymbol(const RDSurface* sf, RDSymbol* symbol) { return CPTR(const Surface, sf)->currentSymbol(symbol); }
bool RDSurface_GetSymbolAt(const RDSurface* sf, int row, int col, RDSymbol* symbol) { return CPTR(const Surface, sf)->symbolAt(row, col, symbol); }
bool RDSurface_GoTo(RDSurface* sf, const RDDocumentItem* item) { return CPTR(Surface, sf)->goTo(item); }
bool RDSurface_GoToAddress(RDSurface* sf, rd_address address) { return CPTR(Surface, sf)->goToAddress(address); }
bool RDSurface_HasSelection(const RDSurface* sf) { return CPTR(const Surface, sf)->hasSelection(); }
bool RDSurface_CanGoBack(const RDSurface* sf) { return CPTR(const Surface, sf)->canGoBack(); }
bool RDSurface_CanGoForward(const RDSurface* sf) { return CPTR(const Surface, sf)->canGoForward(); }
bool RDSurface_GetCurrentItem(const RDSurface* sf, RDDocumentItem* item) { return CPTR(const Surface, sf)->currentItem(item); }
void RDSurface_GetSize(const RDSurface* sf, int* rows, int* cols) { return CPTR(const Surface, sf)->getSize(rows, cols); }
void RDSurface_Update(RDSurface* sf) { CPTR(Surface, sf)->update(); }
void RDSurface_Scroll(RDSurface* sf, int nrows, int ncols) { CPTR(Surface, sf)->scroll(nrows, ncols); }
void RDSurface_Resize(RDSurface* sf, int rows, int cols) { CPTR(Surface, sf)->resize(rows, cols); }
void RDSurface_MoveTo(RDSurface* sf, int row, int col) { CPTR(Surface, sf)->moveTo(row, col); }
void RDSurface_Select(RDSurface* sf, int row, int col) { CPTR(Surface, sf)->select(row, col); }
void RDSurface_SelectAt(RDSurface* sf, int row, int col) { CPTR(Surface, sf)->selectAt(row, col); }
void RDSurface_GoBack(RDSurface* sf) { return CPTR(Surface, sf)->goBack(); }
void RDSurface_GoForward(RDSurface* sf) { return CPTR(Surface, sf)->goForward(); }
void RDSurface_Activate(RDSurface* sf) { CPTR(Surface, sf)->enable(); }
void RDSurface_Deactivate(RDSurface* sf) { CPTR(Surface, sf)->disable(); }

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
