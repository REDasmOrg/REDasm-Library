#include "renderer.h"
#include <rdcore/renderer/renderer.h>

RDRenderer* RDRenderer_Create(RDDisassembler* d, const RDCursor* cursor, rd_flag flags) { return CPTR(RDRenderer, new Renderer(CPTR(Disassembler, d), CPTR(const Cursor, cursor), flags)); }
RDRendererItem* RDRender_CreateItem() { return CPTR(RDRendererItem, new RendererItem()); }
size_t RDRenderer_GetLastColumn(const RDRenderer* r, size_t index) { return CPTR(const Renderer, r)->getLastColumn(index); }
rd_flag RDRenderer_Flags(const RDRenderer* r) { return CPTR(const Renderer, r)->flags(); }
const char* RDRenderer_GetWordFromPosition(const RDRenderer* r, const RDCursorPos* pos, RDCursorRange* range) { return CPTR(const Renderer, r)->getWordFromPosition(pos, range).c_str(); }
const char* RDRenderer_GetCurrentWord(const RDRenderer* r) { return CPTR(const Renderer, r)->getCurrentWord().c_str(); }
const char* RDRenderer_GetSelectedText(const RDRenderer* r) { return CPTR(const Renderer, r)->getSelectedText().c_str(); }
const char* RDRenderer_GetInstructionText(const RDRenderer* r, rd_address address) { return CPTR(const Renderer, r)->getInstructionText(address).c_str(); }
bool RDRenderer_GetSelectedSymbol(const RDRenderer* r, RDSymbol* symbol) { return CPTR(const Renderer, r)->selectedSymbol(symbol); }
bool RDRenderer_GetItem(const RDRenderer* r, size_t index, RDRendererItem* ritem) { return CPTR(const Renderer, r)->renderItem(index, CPTR(RendererItem, ritem)); }
void RDRenderer_GetItems(const RDRenderer* r, size_t index, size_t count, Callback_Render render, void* userdata) { CPTR(const Renderer, r)->render(index, count, render, userdata); }
size_t RDRendererItem_GetItemFormats(const RDRendererItem* ritem, const RDRendererFormat** formats) { return CPTR(const RendererItem, ritem)->formats(formats); }
size_t RDRendererItem_GetDocumentIndex(const RDRendererItem* ritem) { return CPTR(const RendererItem, ritem)->documentIndex(); }
const char* RDRendererItem_GetItemText(const RDRendererItem* ritem) { return CPTR(const RendererItem, ritem)->text().c_str(); }

void RDRenderer_Indent(const RDRenderItemParams* rip, size_t n) { CPTR(const Renderer, rip->renderer)->renderIndent(CPTR(RendererItem, rip->rendereritem), n); }
void RDRenderer_HexDump(const RDRenderItemParams* rip, const RDBufferView* view, size_t size) { CPTR(const Renderer, rip->renderer)->renderHexDump(CPTR(RendererItem, rip->rendereritem), view, size); }
void RDRenderer_Address(const RDRenderItemParams* rip, rd_address address) { CPTR(const Renderer, rip->renderer)->renderAddress(CPTR(RendererItem, rip->rendereritem), address); }
void RDRenderer_Constant(const RDRenderItemParams* rip, const char* s) { CPTR(const Renderer, rip->renderer)->renderConstant(CPTR(RendererItem, rip->rendereritem), s ? s : std::string()); }
void RDRenderer_Mnemonic(const RDRenderItemParams* rip, const char* s, rd_type theme) { CPTR(const Renderer, rip->renderer)->renderMnemonic(CPTR(RendererItem, rip->rendereritem), s ? s : std::string(), theme); }
void RDRenderer_Register(const RDRenderItemParams* rip, const char* s) { CPTR(const Renderer, rip->renderer)->renderRegister(CPTR(RendererItem, rip->rendereritem), s ? s : std::string()); }
void RDRenderer_Text(const RDRenderItemParams* rip, const char* s) { CPTR(const Renderer, rip->renderer)->renderText(CPTR(RendererItem, rip->rendereritem), s ? s : std::string()); }

