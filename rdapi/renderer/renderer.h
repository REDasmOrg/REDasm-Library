#pragma once

#include "../types.h"
#include "../buffer.h"

struct RDContext;

RD_HANDLE(RDRenderer);

enum RDRendererFlags {
    RendererFlags_None             = 0,

    RendererFlags_Decompile        = (1 << 1),
    RendererFlags_CenterOnSurface  = (1 << 2),

    RendererFlags_NoSegment        = (1 << 3),
    RendererFlags_NoAddress        = (1 << 4),
    RendererFlags_NoSeparators     = (1 << 5),
    RendererFlags_NoIndent         = (1 << 6),
    RendererFlags_NoCursor         = (1 << 7),
    RendererFlags_NoHighlightWords = (1 << 8),
    RendererFlags_NoComments       = (1 << 9),

    RendererFlags_NoSegmentAndAddress = RendererFlags_NoSegment | RendererFlags_NoAddress,
    RendererFlags_Simplified          = ~0 & ~(RendererFlags_Decompile | RendererFlags_CenterOnSurface),
};

typedef struct RDRendererParams {
    rd_address address;
    RDBufferView view;

    const RDContext* context;
    RDRenderer* renderer;
} RDRendererParams;

RD_API_EXPORT void RDRenderer_Indent(RDRenderer* r, size_t n);
RD_API_EXPORT void RDRenderer_HexDump(RDRenderer* r, const RDBufferView* view, size_t size);
RD_API_EXPORT void RDRenderer_Signed(RDRenderer* r, s64 val);
RD_API_EXPORT void RDRenderer_Unsigned(RDRenderer* r, u64 val);
RD_API_EXPORT void RDRenderer_Mnemonic(RDRenderer* r, const char* s, rd_type theme);
RD_API_EXPORT void RDRenderer_MnemonicWord(RDRenderer* r, const char* s, rd_type theme);
RD_API_EXPORT void RDRenderer_Register(RDRenderer* r, const char* s);
RD_API_EXPORT void RDRenderer_Constant(RDRenderer* r, const char* s);
RD_API_EXPORT void RDRenderer_Text(RDRenderer* r, const char* s);
RD_API_EXPORT void RDRenderer_Unknown(RDRenderer* r);

RD_API_EXPORT const char* RD_GetInstruction(RDContext* ctx, rd_address address);
RD_API_EXPORT const char* RD_GetAssemblerInstruction(RDContext* ctx, rd_address address);
RD_API_EXPORT const char* RD_GetRDILInstruction(RDContext* ctx, rd_address address);
