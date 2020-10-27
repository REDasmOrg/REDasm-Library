#pragma once

#include "../types.h"
#include "../buffer.h"

struct RDContext;

DECLARE_HANDLE(RDRenderer);

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
RD_API_EXPORT void RDRenderer_Register(RDRenderer* r, const char* s);
RD_API_EXPORT void RDRenderer_Constant(RDRenderer* r, const char* s);
RD_API_EXPORT void RDRenderer_Text(RDRenderer* r, const char* s);
