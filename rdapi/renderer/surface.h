#pragma once

#include "../types.h"
#include "../document/document.h"
#include "renderer.h"

struct RDSymbol;
struct RDContext;
struct RDDocumentItem;

RD_HANDLE(RDSurface);

typedef struct RDSurfaceCell {
    u8 background;
    u8 foreground;
    char ch;
} RDSurfaceCell;

typedef struct RDSurfacePos {
    int row;
    int col;
} RDSurfacePos;

typedef struct RDPathItem {
    int fromrow;  // = -1 if out of range
    int torow;    // = rows + 1 if out of range
    u8 style;
} RDPathItem;

RD_API_EXPORT RDSurface* RDSurface_Create(RDContext* ctx, rd_flag flags, uintptr_t userdata);
RD_API_EXPORT const RDSurfacePos* RDSurface_GetPosition(const RDSurface* sf);
RD_API_EXPORT const RDSurfacePos* RDSurface_GetSelection(const RDSurface* sf);
RD_API_EXPORT const RDSurfacePos* RDSurface_GetStartSelection(const RDSurface* sf);
RD_API_EXPORT const RDSurfacePos* RDSurface_GetEndSelection(const RDSurface* sf);
RD_API_EXPORT const RDDocumentItem* RDSurface_GetFirstItem(const RDSurface* sf);
RD_API_EXPORT const RDDocumentItem* RDSurface_GetLastItem(const RDSurface* sf);
RD_API_EXPORT const char* RDSurface_GetSelectedText(const RDSurface* sf);
RD_API_EXPORT const char* RDSurface_GetCurrentWord(const RDSurface* sf);
RD_API_EXPORT const char* RDSurface_GetWordAt(const RDSurface* sf, int row, int col);
RD_API_EXPORT uintptr_t RDSurface_GetUserData(const RDSurface* sf);
RD_API_EXPORT size_t RDSurface_GetPath(const RDSurface* sf, const RDPathItem** path);
RD_API_EXPORT int RDSurface_GetScrollLength(const RDSurface* sf);
RD_API_EXPORT int RDSurface_GetScrollValue(const RDSurface* sf);
RD_API_EXPORT int RDSurface_GetRow(const RDSurface* sf, int row, const RDSurfaceCell** cells);
RD_API_EXPORT int RDSurface_FindRow(const RDSurface* sf, const RDDocumentItem* item);
RD_API_EXPORT bool RDSurface_GetItem(const RDSurface* sf, int row, RDDocumentItem* item);
RD_API_EXPORT bool RDSurface_Contains(const RDSurface* sf, const RDDocumentItem* item);
RD_API_EXPORT bool RDSurface_ContainsAddress(const RDSurface* sf, rd_address address);
RD_API_EXPORT bool RDSurface_GetCurrentSymbol(const RDSurface* sf, RDSymbol* symbol);
RD_API_EXPORT bool RDSurface_GetSymbolAt(const RDSurface* sf, int row, int col, RDSymbol* symbol);
RD_API_EXPORT bool RDSurface_Seek(RDSurface* sf, const RDDocumentItem* item);
RD_API_EXPORT bool RDSurface_GoTo(RDSurface* sf, const RDDocumentItem* item);
RD_API_EXPORT bool RDSurface_GoToAddress(RDSurface* sf, rd_address address);
RD_API_EXPORT void RDSurface_GetSize(const RDSurface* sf, int* rows, int* cols);
RD_API_EXPORT void RDSurface_Update(RDSurface* sf);
RD_API_EXPORT void RDSurface_Scroll(RDSurface* sf, int nrows, int ncols);
RD_API_EXPORT void RDSurface_Resize(RDSurface* sf, int rows, int cols);
RD_API_EXPORT void RDSurface_MoveTo(RDSurface* sf, int row, int col);
RD_API_EXPORT void RDSurface_Select(RDSurface* sf, int row, int col);
RD_API_EXPORT void RDSurface_SelectAt(RDSurface* sf, int row, int col);
RD_API_EXPORT void RDSurface_GoBack(RDSurface* sf);
RD_API_EXPORT void RDSurface_GoForward(RDSurface* sf);
RD_API_EXPORT void RDSurface_Activate(RDSurface* sf);
RD_API_EXPORT void RDSurface_Deactivate(RDSurface* sf);
RD_API_EXPORT void RDSurface_LinkTo(RDSurface* sf, RDSurface* s);
RD_API_EXPORT void RDSurface_Unlink(RDSurface* sf);
RD_API_EXPORT bool RDSurface_GetCurrentItem(const RDSurface* sf, RDDocumentItem* item);
RD_API_EXPORT bool RDSurface_HasSelection(const RDSurface* sf);
RD_API_EXPORT bool RDSurface_CanGoBack(const RDSurface* sf);
RD_API_EXPORT bool RDSurface_CanGoForward(const RDSurface* sf);
