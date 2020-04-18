#pragma once

#include "../macros.h"
#include "../types.h"

DECLARE_HANDLE(RDCursor);

typedef struct RDCursorPos {
    size_t line, column;
} RDCursorPos;

typedef struct RDCursorRange {
    s32 start, end;
} RDCursorRange;

struct RDDocument;

RD_API_EXTERN_C RDCursor* RDCursor_Create(RDDocument* d);
RD_API_EXTERN_C void RDCursor_GoBack(RDCursor* c);
RD_API_EXTERN_C void RDCursor_GoForward(RDCursor* c);
RD_API_EXTERN_C void RDCursor_Enable(RDCursor* c);
RD_API_EXTERN_C void RDCursor_Disable(RDCursor* c);
RD_API_EXTERN_C void RDCursor_Toggle(RDCursor* c);
RD_API_EXTERN_C void RDCursor_ClearSelection(RDCursor* c);
RD_API_EXTERN_C void RDCursor_MoveTo(RDCursor* c, size_t line, size_t column);
RD_API_EXTERN_C void RDCursor_Select(RDCursor* c, size_t line, size_t column);
RD_API_EXTERN_C bool RDCursor_IsLineSelected(const RDCursor* c, size_t line);
RD_API_EXTERN_C bool RDCursor_HasSelection(const RDCursor* c);
RD_API_EXTERN_C bool RDCursor_CanGoBack(const RDCursor* c);
RD_API_EXTERN_C bool RDCursor_CanGoForward(const RDCursor* c);
RD_API_EXTERN_C size_t RDCursor_CurrentLine(const RDCursor* c);
RD_API_EXTERN_C size_t RDCursor_CurrentColumn(const RDCursor* c);
RD_API_EXTERN_C size_t RDCursor_SelectionLine(const RDCursor* c);
RD_API_EXTERN_C size_t RDCursor_SelectionColumn(const RDCursor* c);
RD_API_EXTERN_C const RDCursorPos* RDCursor_GetPosition(const RDCursor* c);
RD_API_EXTERN_C const RDCursorPos* RDCursor_GetSelection(const RDCursor* c);
RD_API_EXTERN_C const RDCursorPos* RDCursor_GetStartSelection(const RDCursor* c);
RD_API_EXTERN_C const RDCursorPos* RDCursor_GetEndSelection(const RDCursor* c);
