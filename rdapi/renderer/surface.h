#pragma once

#include "../types.h"
#include "../document/cursor.h"

struct RDSymbol;
struct RDContext;
struct RDDocumentItem;

DECLARE_HANDLE(RDSurface);

enum RDSurfaceFlags {
    SurfaceFlags_Normal           = 0,

    SurfaceFlags_Decompile        = (1 << 1),
    SurfaceFlags_NoSegment        = (1 << 2),
    SurfaceFlags_NoAddress        = (1 << 3),
    SurfaceFlags_NoSeparators     = (1 << 4),
    SurfaceFlags_NoIndent         = (1 << 5),
    SurfaceFlags_NoCursor         = (1 << 6),
    SurfaceFlags_NoHighlightWords = (1 << 7),
    SurfaceFlags_NoComments       = (1 << 8),

    SurfaceFlags_NoSegmentAndAddress = SurfaceFlags_NoSegment | SurfaceFlags_NoAddress,
    SurfaceFlags_Simplified          = ~0,
};

typedef struct RDSurfaceCell {
    u8 background;
    u8 foreground;
    char ch;
} RDSurfaceCell;

RD_API_EXPORT RDSurface* RDSurface_Create(RDContext* ctx, rd_flag flags);
RD_API_EXPORT const RDCursorPos* RDSurface_GetPosition(const RDSurface* s);
RD_API_EXPORT const RDCursorPos* RDSurface_GetSelection(const RDSurface* s);
RD_API_EXPORT const RDCursorPos* RDSurface_GetStartSelection(const RDSurface* s);
RD_API_EXPORT const RDCursorPos* RDSurface_GetEndSelection(const RDSurface* s);
RD_API_EXPORT const char* RDSurface_GetCurrentWord(const RDSurface* sf);
RD_API_EXPORT int RDSurface_GetRow(const RDSurface* sf, int row, RDSurfaceCell* cells);
RD_API_EXPORT bool RDSurface_GetSelectedSymbol(const RDSurface* sf, RDSymbol* symbol);
RD_API_EXPORT bool RDSurface_GoTo(RDSurface* sf, const RDDocumentItem* item);
RD_API_EXPORT bool RDSurface_GoToAddress(RDSurface* sf, rd_address address);
RD_API_EXPORT void RDSurface_GetSize(const RDSurface* sf, int* rows, int* cols);
RD_API_EXPORT void RDSurface_Scroll(RDSurface* sf, int nrows, int ncols);
RD_API_EXPORT void RDSurface_Resize(RDSurface* sf, int rows, int cols);
RD_API_EXPORT void RDSurface_MoveTo(RDSurface* sf, int row, int col);
RD_API_EXPORT void RDSurface_Select(RDSurface* sf, int row, int col);
RD_API_EXPORT void RDSurface_GoBack(RDSurface* sf);
RD_API_EXPORT void RDSurface_GoForward(RDSurface* sf);
RD_API_EXPORT void RDSurface_EnableCursor(RDSurface* sf);
RD_API_EXPORT void RDSurface_DisableCursor(RDSurface* sf);
RD_API_EXPORT bool RDSurface_GetCurrentItem(const RDSurface* sf, RDDocumentItem* item);
RD_API_EXPORT bool RDSurface_HasSelection(const RDSurface* sf);
RD_API_EXPORT bool RDSurface_CanGoBack(const RDSurface* sf);
RD_API_EXPORT bool RDSurface_CanGoForward(const RDSurface* sf);
