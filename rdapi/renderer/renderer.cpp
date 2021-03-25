#include "renderer.h"
#include <rdcore/surface/renderer.h>

void RDRenderer_Indent(RDRenderer* r, size_t n) { CPTR(Renderer, r)->renderIndent(n, true); }
void RDRenderer_Signed(RDRenderer* r, s64 val) {  CPTR(Renderer, r)->renderSigned(val);  }
void RDRenderer_Unsigned(RDRenderer* r, u64 val) { CPTR(Renderer, r)->renderUnsigned(val); }
void RDRenderer_Reference(RDRenderer* r, rd_location ref) { CPTR(Renderer, r)->renderReference(ref); }
void RDRenderer_Mnemonic(RDRenderer* r, const char* s, rd_type theme) { CPTR(Renderer, r)->renderMnemonic(s, theme); }
void RDRenderer_MnemonicWord(RDRenderer* r, const char* s, rd_type theme) { CPTR(Renderer, r)->renderMnemonicWord(s, theme); }
void RDRenderer_Register(RDRenderer* r, const char* s) { CPTR(Renderer, r)->renderRegister(s); }
void RDRenderer_Constant(RDRenderer* r, const char* s) { CPTR(Renderer, r)->renderConstant(s); }
void RDRenderer_Text(RDRenderer* r, const char* s) { CPTR(Renderer, r)->renderText(s); }
void RDRenderer_Themed(RDRenderer* r, const char* s, rd_type theme) { CPTR(Renderer, r)->renderText(s, theme); }
void RDRenderer_Unknown(RDRenderer* r) { CPTR(Renderer, r)->renderUnknown(); }

const char* RD_GetInstruction(RDContext* ctx, rd_address address)
{
    static std::string s;
    s = Renderer::getInstruction(CPTR(Context, ctx), address);
    return !s.empty() ? s.c_str() : nullptr;
}

const char* RD_GetAssemblerInstruction(RDContext* ctx, rd_address address)
{
    static std::string s;
    s = Renderer::getAssemblerInstruction(CPTR(Context, ctx), address);
    return !s.empty() ? s.c_str() : nullptr;
}

const char* RD_GetRDILInstruction(RDContext* ctx, rd_address address)
{
    static std::string s;
    s = Renderer::getRDILInstruction(CPTR(Context, ctx), address);
    return !s.empty() ? s.c_str() : nullptr;
}
