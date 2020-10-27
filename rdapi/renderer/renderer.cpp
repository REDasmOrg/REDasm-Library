#include "renderer.h"
#include <rdcore/renderer/renderer.h>

void RDRenderer_Indent(RDRenderer* r, size_t n) { CPTR(Renderer, r)->renderIndent(n, true); }
void RDRenderer_HexDump(RDRenderer* r, const RDBufferView* view, size_t size) { CPTR(Renderer, r)->renderHexDump(view, size); }
void RDRenderer_Signed(RDRenderer* r, s64 val) {  CPTR(Renderer, r)->renderSigned(val);  }
void RDRenderer_Unsigned(RDRenderer* r, u64 val) { CPTR(Renderer, r)->renderUnsigned(val); }
void RDRenderer_Mnemonic(RDRenderer* r, const char* s, rd_type theme) { CPTR(Renderer, r)->renderMnemonic(s, theme); }
void RDRenderer_Register(RDRenderer* r, const char* s) { CPTR(Renderer, r)->renderRegister(s); }
void RDRenderer_Constant(RDRenderer* r, const char* s) { CPTR(Renderer, r)->renderConstant(s); }
void RDRenderer_Text(RDRenderer* r, const char* s) { CPTR(Renderer, r)->renderText(s); }
