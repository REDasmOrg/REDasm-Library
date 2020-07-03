#include "renderer.h"
#include <rdcore/renderer/renderer.h>

RDRenderer* RDRenderer_Create(RDDisassembler* d, const RDCursor* cursor, rd_flag flags) { return CPTR(RDRenderer, new Renderer(CPTR(Disassembler, d), CPTR(const Cursor, cursor), flags)); }
RDRendererItem* RDRender_CreateItem() { return CPTR(RDRendererItem, new RendererItem()); }
size_t RDRenderer_GetLastColumn(const RDRenderer* r, size_t index) { return CPTR(const Renderer, r)->getLastColumn(index); }
rd_flag RDRenderer_Flags(const RDRenderer* r) { return CPTR(const Renderer, r)->flags(); }
const char* RDRenderer_GetWordFromPosition(const RDRenderer* r, const RDCursorPos* pos, RDCursorRange* range) { return CPTR(const Renderer, r)->getWordFromPosition(pos, range).c_str(); }
const char* RDRenderer_GetCurrentWord(const RDRenderer* r) { return CPTR(const Renderer, r)->getCurrentWord().c_str(); }
const char* RDRenderer_GetSelectedText(const RDRenderer* r) { return CPTR(const Renderer, r)->getSelectedText().c_str(); }
const char* RDRenderer_GetInstruction(const RDRenderer* r, rd_address address) { return CPTR(const Renderer, r)->getInstruction(address).c_str(); }
bool RDRenderer_GetSelectedSymbol(const RDRenderer* r, RDSymbol* symbol) { return CPTR(const Renderer, r)->selectedSymbol(symbol); }
bool RDRenderer_GetItem(const RDRenderer* r, size_t index, RDRendererItem* ritem) { return CPTR(const Renderer, r)->renderItem(index, ritem); }
void RDRenderer_GetItems(const RDRenderer* r, size_t index, size_t count, Callback_Render render, void* userdata) { return CPTR(const Renderer, r)->render(index, count, render, userdata); }
size_t RDRendererItem_GetItemFormats(const RDRendererItem* ritem, const RDRendererFormat** formats) { return CPTR(const RendererItem, ritem)->formats(formats); }
size_t RDRendererItem_GetDocumentIndex(const RDRendererItem* ritem) { return CPTR(const RendererItem, ritem)->documentIndex(); }
const char* RDRendererItem_GetItemText(const RDRendererItem* ritem) { return CPTR(const RendererItem, ritem)->text().c_str(); }
void RDRendererItem_Push(RDRendererItem* ritem, const char* s, const char* fgstyle, const char* bgstyle) { CPTR(RendererItem, ritem)->push(s, fgstyle ? fgstyle : std::string(), bgstyle ? bgstyle : std::string()); }
void RDRenderer_Prologue(RDRenderItemParams* rip) { Renderer::renderPrologue(rip); }
void RDRenderer_Address(RDRenderItemParams* rip) { Renderer::renderAddress(rip); }
void RDRenderer_AddressIndent(RDRenderItemParams* rip) { Renderer::renderAddressIndent(rip); }
void RDRenderer_Constant(RDRenderItemParams* rip, u64 c) { Renderer::renderConstant(rip, c); }
void RDRenderer_Immediate(RDRenderItemParams* rip, u64 imm) { Renderer::renderImmediate(rip, imm); }
void RDRenderer_Register(RDRenderItemParams* rip, const RDOperand* op, rd_register_id r) { Renderer::renderRegister(rip, op, r); }
void RDRenderer_Mnemonic(RDRenderItemParams* rip) { Renderer::renderMnemonic(rip); }
void RDRenderer_Operand(RDRenderItemParams* rip, const RDOperand* op) { Renderer::renderOperand(rip, op); }
void RDRenderer_Text(RDRenderItemParams* rip, const char* s) { Renderer::renderText(rip, s); }
void RDRenderer_Indent(RDRenderItemParams* rip, size_t n) { Renderer::renderIndent(rip, n); }

