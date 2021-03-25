#pragma once

#include "../types.h"
#include "renderer.h"

struct RDContext;

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
RD_API_EXPORT rd_address RDSurface_GetFirstAddress(const RDSurface* sf);
RD_API_EXPORT rd_address RDSurface_GetLastAddress(const RDSurface* sf);
RD_API_EXPORT rd_address RDSurface_GetCurrentAddress(const RDSurface* sf);
RD_API_EXPORT rd_address RDSurface_GetAddress(const RDSurface* sf, int row);
RD_API_EXPORT const char* RDSurface_GetSelectedText(const RDSurface* sf);
RD_API_EXPORT const char* RDSurface_GetCurrentWord(const RDSurface* sf);
RD_API_EXPORT const char* RDSurface_GetCurrentLabel(const RDSurface* sf, rd_address* address);
RD_API_EXPORT const char* RDSurface_GetWordAt(const RDSurface* sf, int row, int col);
RD_API_EXPORT uintptr_t RDSurface_GetUserData(const RDSurface* sf);
RD_API_EXPORT size_t RDSurface_GetPath(const RDSurface* sf, const RDPathItem** path);
RD_API_EXPORT int RDSurface_GetRow(const RDSurface* sf, int row, const RDSurfaceCell** cells);
RD_API_EXPORT int RDSurface_IndexOf(const RDSurface* sf, rd_address address);
RD_API_EXPORT int RDSurface_LastIndexOf(const RDSurface* sf, rd_address address);
RD_API_EXPORT bool RDSurface_Contains(const RDSurface* sf, const rd_address address);
RD_API_EXPORT bool RDSurface_GetLabelAt(const RDSurface* sf, int row, int col, rd_address* address);
RD_API_EXPORT bool RDSurface_Seek(RDSurface* sf, rd_address address);
RD_API_EXPORT bool RDSurface_GoTo(RDSurface* sf, rd_address address);
RD_API_EXPORT void RDSurface_GetSize(const RDSurface* sf, int* rows, int* cols);
RD_API_EXPORT void RDSurface_GetScrollRange(const RDSurface* sf, rd_address* start, rd_address* end);
RD_API_EXPORT void RDSurface_Update(RDSurface* sf);
RD_API_EXPORT void RDSurface_Scroll(RDSurface* sf, rd_address address, int ncols);
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
RD_API_EXPORT bool RDSurface_HasSelection(const RDSurface* sf);
RD_API_EXPORT bool RDSurface_CanGoBack(const RDSurface* sf);
RD_API_EXPORT bool RDSurface_CanGoForward(const RDSurface* sf);
