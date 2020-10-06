#include "context.h"
#include <rdcore/document/document.h>
#include <rdcore/context.h>

RDContext* RDContext_Create() { return CPTR(RDContext, new Context()); }
const RDEntryAssembler* RDContext_FindAssemblerEntry(const RDContext* ctx, const RDEntryLoader* entryloader) { return CPTR(const Context, ctx)->findAssemblerEntry(entryloader, nullptr); }
RDDisassembler* RDContext_BuildDisassembler(RDContext* ctx, const RDLoaderRequest* req, const RDEntryLoader* entryloader, const RDEntryAssembler* entryassembler) { return CPTR(RDDisassembler, CPTR(Context, ctx)->buildDisassembler(req, entryloader, entryassembler)); }
const RDNet* RDContext_GetNet(const RDContext* ctx) { return CPTR(const RDNet, CPTR(const Context, ctx)->net()); }
RDDocument* RDContext_GetDocument(const RDContext* ctx) { return CPTR(RDDocument, std::addressof(CPTR(const Context, ctx)->document())); }
RDDisassembler* RDContext_GetDisassembler(const RDContext* ctx) { return CPTR(RDDisassembler, CPTR(const Context, ctx)->disassembler()); }
RDAssembler* RDContext_GetAssembler(const RDContext* ctx) { return CPTR(RDAssembler, CPTR(const Context, ctx)->assembler()); }
RDLoader* RDContext_GetLoader(const RDContext* ctx) { return CPTR(RDLoader, CPTR(const Context, ctx)->loader()); }
RDBuffer* RDContext_GetBuffer(const RDContext* ctx) { return CPTR(RDBuffer, CPTR(const Context, ctx)->buffer()); }
size_t RDContext_GetBits(const RDContext* ctx) { return CPTR(const Context, ctx)->bits(); }
size_t RDContext_GetAddressWidth(const RDContext* ctx) { return CPTR(const Context, ctx)->addressWidth(); }
void RDContext_SetUserData(RDContext* ctx, const char* s, uintptr_t userdata) { if(s) CPTR(Context, ctx)->setUserData(s, userdata); }
uintptr_t RDContext_GetUserData(const RDContext* ctx, const char* s) { return s ? CPTR(const Context, ctx)->getUserData(s) : 0; }
bool RDContext_IsBusy(const RDContext* ctx) { return CPTR(const Context, ctx)->busy(); }
void RDContext_FindLoaderEntries(RDContext* ctx, const RDLoaderRequest* loadrequest, Callback_LoaderEntry callback, void* userdata) { CPTR(Context, ctx)->findLoaderEntries(loadrequest, callback, userdata); }
void RDContext_FindAssemblerEntries(const RDContext* ctx, Callback_AssemblerEntry callback, void* userdata) { CPTR(const Context, ctx)->findAssemblerEntries(callback, userdata); }
void RDContext_GetAnalyzers(const RDContext* ctx, Callback_Analyzer callback, void* userdata) { CPTR(const Context, ctx)->getAnalyzers(callback, userdata); }
void RDContext_SelectAnalyzer(RDContext* ctx, const RDAnalyzer* analyzer, bool select) { CPTR(Context, ctx)->selectAnalyzer(CPTR(const Analyzer, analyzer), select); }
void RDContext_Subscribe(RDContext* ctx, void* owner, Callback_Event listener, void* userdata) { CPTR(Context, ctx)->subscribe(owner, listener, userdata); }
void RDContext_Unsubscribe(RDContext* ctx, void* owner) { CPTR(Context, ctx)->unsubscribe(owner); }
void RDContext_DisassembleAt(RDContext* ctx, rd_address address) { CPTR(Context, ctx)->disassembleAt(address); }
void RDContext_Disassemble(RDContext* ctx) { CPTR(Context, ctx)->disassemble(); }
void RDContext_GetProblems(const RDContext* ctx, RD_ProblemCallback callback, void* userdata) { CPTR(const Context, ctx)->getProblems(callback, userdata); }
void RDContext_AddProblem(RDContext* ctx, const char* s) { CPTR(Context, ctx)->problem(s); }
void RDContext_SetIgnoreProblems(RDContext* ctx, bool ignore) { CPTR(Context, ctx)->setIgnoreProblems(ignore); }
bool RDContext_HasProblems(const RDContext* ctx) { return CPTR(const Context, ctx)->hasProblems(); }

void RD_SetContextFlags(rd_flag flags, bool set) {}
bool RD_ContextHasFlags(rd_flag flags) { return false; }
rd_flag RD_GetContextFlags(void) { return 0; }
