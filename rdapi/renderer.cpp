#include "renderer.h"
#include <rdcore/renderer/renderer.h>

RDRenderer* RDRenderer_Create(RDDisassembler* d, const RDCursor* cursor, flag_t flags) { return CPTR(RDRenderer, new Renderer(CPTR(Disassembler, d), CPTR(const Cursor, cursor), flags)); }
RDRendererItem* RDRender_CreateItem() { return CPTR(RDRendererItem, new RendererItem()); }
size_t RDRenderer_GetLastColumn(const RDRenderer* r, size_t index) { return CPTR(const Renderer, r)->getLastColumn(index); }
flag_t RDRenderer_Flags(const RDRenderer* r) { return CPTR(const Renderer, r)->flags(); }
const char* RDRenderer_GetWordFromPosition(const RDRenderer* r, const RDCursorPos* pos, RDCursorRange* range) { return CPTR(const Renderer, r)->getWordFromPosition(pos, range).c_str(); }
const char* RDRenderer_GetCurrentWord(const RDRenderer* r) { return CPTR(const Renderer, r)->getCurrentWord().c_str(); }
const char* RDRenderer_GetSelectedText(const RDRenderer* r) { return CPTR(const Renderer, r)->getSelectedText().c_str(); }
const char* RDRenderer_GetInstruction(const RDRenderer* r, address_t address) { return CPTR(const Renderer, r)->getInstruction(address).c_str(); }
bool RDRenderer_GetSelectedSymbol(const RDRenderer* r, RDSymbol* symbol) { return CPTR(const Renderer, r)->selectedSymbol(symbol); }
bool RDRenderer_GetItem(const RDRenderer* r, size_t index, RDRendererItem* ritem) { return CPTR(const Renderer, r)->renderItem(index, ritem); }
void RDRenderer_GetItems(const RDRenderer* r, size_t index, size_t count, Callback_Render render, void* userdata) { return CPTR(const Renderer, r)->render(index, count, render, userdata); }
void RDRenderer_Immediate(const RDRenderer* r, RDRenderItemParams* rip) { CPTR(const Renderer, r)->renderImmediate(rip); }
size_t RDRendererItem_GetItemFormats(const RDRendererItem* ritem, const RDRendererFormat** formats) { return CPTR(const RendererItem, ritem)->formats(formats); }
size_t RDRendererItem_GetDocumentIndex(const RDRendererItem* ritem) { return CPTR(const RendererItem, ritem)->documentIndex(); }
const char* RDRendererItem_GetItemText(const RDRendererItem* ritem) { return CPTR(const RendererItem, ritem)->text().c_str(); }
void RDRendererItem_Push(RDRendererItem* ritem, const char* s, const char* fgstyle, const char* bgstyle) { CPTR(RendererItem, ritem)->push(s, fgstyle ? fgstyle : std::string(), bgstyle ? bgstyle : std::string()); }

void RD_RendererAddress(RDRenderItemParams* rip) { Renderer::renderAddress(rip); }
void RD_RenderAddressIndent(RDRenderItemParams* rip) { Renderer::renderAddressIndent(rip); }
void RD_RenderIndent(RDRenderItemParams* rip, size_t n) { Renderer::renderIndent(rip, n); }
