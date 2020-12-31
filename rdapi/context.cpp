#include "context.h"
#include <rdcore/document/document.h>
#include <rdcore/disassembler.h>
#include <rdcore/context.h>

RDContext* RDContext_Create() { return CPTR(RDContext, new Context()); }
RDDatabase* RDContext_GetDatabase(const RDContext* ctx) { return CPTR(RDDatabase, CPTR(const Context, ctx)->database()); }
RDLocation RDContext_GetEntryPoint(const RDContext* ctx) { return CPTR(const Context, ctx)->entryPoint(); }
RDLocation RDContext_Dereference(const RDContext* ctx, rd_address address) { return CPTR(const Context, ctx)->document()->dereference(address); }
RDLocation RDContext_GetFunctionStart(const RDContext* ctx, rd_address address) { return CPTR(const Context, ctx)->functionStart(address); }
const RDEntryAssembler* RDContext_FindAssemblerEntry(const RDContext* ctx, const RDEntryLoader* entryloader) { return CPTR(const Context, ctx)->findAssemblerEntry(entryloader, nullptr); }
bool RDContext_MatchLoader(const RDContext* ctx, const char* q) { return q ? CPTR(const Context, ctx)->matchLoader(q) : false; }
bool RDContext_MatchAssembler(const RDContext* ctx, const char* q) { return q ? CPTR(const Context, ctx)->matchAssembler(q) : false; }
bool RDContext_Bind(RDContext* ctx, const RDLoaderRequest* req, const RDEntryLoader* entryloader, const RDEntryAssembler* entryassembler) { return CPTR(Context, ctx)->bind(req, entryloader, entryassembler); }
const RDNet* RDContext_GetNet(const RDContext* ctx) { return CPTR(const RDNet, CPTR(const Context, ctx)->net()); }
RDDocument* RDContext_GetDocument(const RDContext* ctx) { return CPTR(RDDocument, std::addressof(CPTR(const Context, ctx)->document())); }
RDAssembler* RDContext_GetAssembler(const RDContext* ctx) { return CPTR(RDAssembler, CPTR(const Context, ctx)->assembler()); }
RDLoader* RDContext_GetLoader(const RDContext* ctx) { return CPTR(RDLoader, CPTR(const Context, ctx)->loader()); }
RDBuffer* RDContext_GetBuffer(const RDContext* ctx) { return CPTR(RDBuffer, CPTR(const Context, ctx)->buffer()); }
size_t RDContext_GetBits(const RDContext* ctx) { return CPTR(const Context, ctx)->bits(); }
size_t RDContext_GetAddressWidth(const RDContext* ctx) { return CPTR(const Context, ctx)->addressWidth(); }
RDSurface* RDContext_GetActiveSurface(const RDContext* ctx) { return CPTR(RDSurface, CPTR(const Context, ctx)->activeSurface()); }
void RDContext_SetUserData(RDContext* ctx, const char* s, uintptr_t userdata) { if(s) CPTR(Context, ctx)->setUserData(s, userdata); }
uintptr_t RDContext_GetUserData(const RDContext* ctx, const char* s) { return s ? CPTR(const Context, ctx)->getUserData(s) : 0; }
size_t RDContext_GetMinString(const RDContext* ctx) { return CPTR(const Context, ctx)->minString(); }
void RDContext_SetMinString(RDContext* ctx, size_t n) { return CPTR(Context, ctx)->setMinString(n); }
bool RDContext_IsBusy(const RDContext* ctx) { return CPTR(const Context, ctx)->busy(); }
void RDContext_FindLoaderEntries(RDContext* ctx, const RDLoaderRequest* loadrequest, Callback_LoaderEntry callback, void* userdata) { CPTR(Context, ctx)->findLoaderEntries(loadrequest, callback, userdata); }
void RDContext_FindAssemblerEntries(const RDContext* ctx, Callback_AssemblerEntry callback, void* userdata) { CPTR(const Context, ctx)->findAssemblerEntries(callback, userdata); }
void RDContext_GetAnalyzers(const RDContext* ctx, Callback_Analyzer callback, void* userdata) { CPTR(const Context, ctx)->getAnalyzers(callback, userdata); }
void RDContext_SelectAnalyzer(RDContext* ctx, const RDAnalyzer* analyzer, bool select) { CPTR(Context, ctx)->selectAnalyzer(CPTR(const Analyzer, analyzer), select); }
void RDContext_DisassembleFunction(RDContext* ctx, rd_address address, const char* name) { CPTR(Context, ctx)->disassembler()->disassembleFunction(address, name); }
void RDContext_DisassembleBlock(RDContext* ctx, const RDBlock* block) { CPTR(Context, ctx)->disassembleBlock(block); }
void RDContext_DisassembleAt(RDContext* ctx, rd_address address) { CPTR(Context, ctx)->disassembleAt(address); }
void RDContext_Disassemble(RDContext* ctx) { CPTR(Context, ctx)->disassemble(); }
size_t RDContext_GetProblemsCount(const RDContext* ctx) { return CPTR(const Context, ctx)->problemsCount(); }
void RDContext_GetProblems(const RDContext* ctx, RD_ProblemCallback callback, void* userdata) { CPTR(const Context, ctx)->getProblems(callback, userdata); }
void RDContext_AddProblem(RDContext* ctx, const char* s) { CPTR(Context, ctx)->problem(s); }
void RDContext_SetIgnoreProblems(RDContext* ctx, bool ignore) { CPTR(Context, ctx)->setIgnoreProblems(ignore); }
bool RDContext_HasProblems(const RDContext* ctx) { return CPTR(const Context, ctx)->hasProblems(); }
void RDContext_SetFlags(RDContext* ctx, rd_flag flags, bool set) { CPTR(Context, ctx)->setFlags(flags, set); }
bool RDContext_HasFlag(const RDContext* ctx, rd_flag flag) { return CPTR(const Context, ctx)->hasFlag(flag); }
rd_flag RDContext_GetFlags(const RDContext* ctx) { return CPTR(const Context, ctx)->flags(); }
void RDContext_SetABI(RDContext* ctx, rd_type t) { CPTR(Context, ctx)->setCompilerABI(t); }
rd_type RDContext_GetABI(const RDContext* ctx) { return CPTR(const Context, ctx)->compilerABI(); }
void RDContext_SetCC(RDContext* ctx, rd_type t) { CPTR(Context, ctx)->setCompilerCC(t); }
rd_type RDContext_GetCC(const RDContext* ctx) { return CPTR(const Context, ctx)->compilerCC(); }
const char* RDContext_FunctionHexDump(const RDContext* ctx, rd_address address, RDSymbol* symbol) { return CPTR(const Context, ctx)->disassembler()->getFunctionHexDump(address, symbol); }
bool RDContext_CreateFunction(RDContext* ctx, rd_address address, const char* name) { return CPTR(Context, ctx)->disassembler()->createFunction(address, name); }
void RDContext_Enqueue(RDContext* ctx, rd_address address) { CPTR(Context, ctx)->disassembler()->enqueue(address); }

const char* RD_HexDump(const RDContext* ctx, rd_address address, size_t size) { return CPTR(const Context, ctx)->document()->getHexDump(address, size); }
const char* RD_ReadString(const RDContext* ctx, rd_address address, size_t* len) { return CPTR(const Context, ctx)->document()->readString(address, len); }
const char16_t* RD_ReadWString(const RDContext* ctx, rd_address address, size_t* len) { return CPTR(const Context, ctx)->document()->readWString(address, len); }

bool RDContext_GetSegmentView(const RDContext* ctx, const RDSegment* segment, RDBufferView* view)
{
    if(!segment) return false;
    auto& document = CPTR(const Context, ctx)->document();
    return document->view(*segment, view);
}

bool RDContext_GetBlockView(const RDContext* ctx, const RDBlock* block, RDBufferView* view)
{
    if(!block) return false;
    auto& document = CPTR(const Context, ctx)->document();
    return document->view(*block, view);
}
