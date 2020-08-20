#pragma once

#include "macros.h"
#include "types.h"

enum RDTheme {
    Theme_Default = 0,
    Theme_Foreground, Theme_Background, Theme_Seek, Theme_Comment, Theme_Meta,
    Theme_HighlightFg, Theme_HighlightBg,
    Theme_SelectionFg, Theme_SelectionBg,
    Theme_CursorFg, Theme_CursorBg,
    Theme_Segment, Theme_Function, Theme_Type,
    Theme_Address, Theme_Constant, Theme_Reg,
    Theme_String, Theme_Symbol, Theme_Data, Theme_Pointer, Theme_Imported,
    Theme_Nop, Theme_Ret, Theme_Call, Theme_Jump, Theme_JumpCond,
    Theme_EntryFg, Theme_EntryBg,
    Theme_GraphBg, Theme_GraphEdge, Theme_GraphEdgeTrue, Theme_GraphEdgeFalse, Theme_GraphEdgeLoop, Theme_GraphEdgeLoopCond
};

RD_API_EXPORT void RDTheme_Set(rd_type theme, const char* color);
RD_API_EXPORT const char* RDTheme_Get(rd_type theme);
