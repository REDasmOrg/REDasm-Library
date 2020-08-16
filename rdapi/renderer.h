#pragma once

#include "macros.h"
#include "types.h"
#include "buffer.h"

DECLARE_HANDLE(RDRenderer);
DECLARE_HANDLE(RDRendererItem);

struct RDBufferView;
struct RDDisassembler;
struct RDDocumentItem;
struct RDCursorRange;
struct RDCursorPos;
struct RDCursor;
struct RDSymbol;

enum RDRendererItemType {
    RendererItemType_Segment,
    RendererItemType_Function,
    RendererItemType_Instruction,
    RendererItemType_Symbol,
    RendererItemType_Operand,
};

enum RDRendererFlags {
    RendererFlags_Normal           = 0,

    RendererFlags_Decompile        = (1 << 1),
    RendererFlags_NoSegment        = (1 << 2),
    RendererFlags_NoAddress        = (1 << 3),
    RendererFlags_NoSeparators     = (1 << 4),
    RendererFlags_NoIndent         = (1 << 5),
    RendererFlags_NoCursor         = (1 << 6),
    RendererFlags_NoHighlightWords = (1 << 7),
    RendererFlags_NoComments       = (1 << 8),

    RendererFlags_NoSegmentAndAddress = RendererFlags_NoSegment | RendererFlags_NoAddress,
    RendererFlags_Simplified          = ~0,
};

typedef struct RDRenderItemParams {
    rd_address address;
    RDBufferView view;

    const RDRenderer* renderer;
    const RDDisassembler* disassembler;
    RDRendererItem* rendereritem;
} RDRenderItemParams;

typedef struct RDRendererFormat {
    s32 start, end; // [start, end]
    rd_type fgtheme, bgtheme;
} RDRendererFormat;

typedef void (*Callback_Render)(const RDRendererItem* ritem, size_t index, void* userdata);

// RDRenderer
RD_API_EXPORT RDRenderer* RDRenderer_Create(RDDisassembler* d, const RDCursor* cursor, rd_flag flags);
RD_API_EXPORT RDRendererItem* RDRender_CreateItem();
RD_API_EXPORT size_t RDRenderer_GetLastColumn(const RDRenderer* r, size_t index);
RD_API_EXPORT rd_flag RDRenderer_Flags(const RDRenderer* r);
RD_API_EXPORT const char* RDRenderer_GetWordFromPosition(const RDRenderer* r, const RDCursorPos* pos, RDCursorRange* range);
RD_API_EXPORT const char* RDRenderer_GetCurrentWord(const RDRenderer* r);
RD_API_EXPORT const char* RDRenderer_GetSelectedText(const RDRenderer* r);
RD_API_EXPORT const char* RDRenderer_GetInstructionText(const RDRenderer* r, rd_address address);
RD_API_EXPORT bool RDRenderer_GetSelectedSymbol(const RDRenderer* r, RDSymbol* symbol);
RD_API_EXPORT bool RDRenderer_GetItem(const RDRenderer* r, size_t index, RDRendererItem* ritem);
RD_API_EXPORT void RDRenderer_GetItems(const RDRenderer* r, size_t index, size_t count, Callback_Render render, void* userdata);

// RDRendererItem
RD_API_EXPORT const char* RDRendererItem_GetItemText(const RDRendererItem* ritem);
RD_API_EXPORT size_t RDRendererItem_GetItemFormats(const RDRendererItem* ritem, const RDRendererFormat** formats);
RD_API_EXPORT size_t RDRendererItem_GetDocumentIndex(const RDRendererItem* ritem);

// Rendering Utilities
RD_API_EXPORT void RDRenderer_Indent(const RDRenderItemParams* rip, size_t n);
RD_API_EXPORT void RDRenderer_HexDump(const RDRenderItemParams* rip, const RDBufferView* view, size_t size);
RD_API_EXPORT void RDRenderer_Address(const RDRenderItemParams* rip, rd_address address);
RD_API_EXPORT void RDRenderer_Constant(const RDRenderItemParams* rip, const char* s);
RD_API_EXPORT void RDRenderer_Mnemonic(const RDRenderItemParams* rip, const char* s, rd_type theme);
RD_API_EXPORT void RDRenderer_Register(const RDRenderItemParams* rip, const char* s);
RD_API_EXPORT void RDRenderer_Text(const RDRenderItemParams* rip, const char* s);
