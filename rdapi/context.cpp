#include "context.h"
#include <rdcore/database/addressdatabase.h>
#include <rdcore/database/contextstate.h>
#include <rdcore/document/document.h>
#include <rdcore/disassembler.h>
#include <rdcore/context.h>

RDContext* RDContext_Create() { return CPTR(RDContext, new Context()); }
RDDatabase* RDContext_GetDatabase(const RDContext* ctx) { return CPTR(RDDatabase, CPTR(const Context, ctx)->database()); }
const RDEntryAssembler* RDContext_FindAssemblerEntry(const RDContext* ctx, const RDEntryLoader* entryloader) { return CPTR(const Context, ctx)->findAssemblerEntry(entryloader, nullptr); }
bool RDContext_MatchLoader(const RDContext* ctx, const char* q) { return q ? CPTR(const Context, ctx)->matchLoader(q) : false; }
bool RDContext_MatchAssembler(const RDContext* ctx, const char* q) { return q ? CPTR(const Context, ctx)->matchAssembler(q) : false; }
bool RDContext_Bind(RDContext* ctx, const RDLoaderRequest* req, const RDEntryLoader* entryloader, const RDEntryAssembler* entryassembler) { return CPTR(Context, ctx)->bind(req, entryloader, entryassembler); }
const RDNet* RDContext_GetNet(const RDContext* ctx) { return CPTR(const RDNet, CPTR(const Context, ctx)->net()); }
RDDocument* RDContext_GetDocument(const RDContext* ctx) { return CPTR(RDDocument, std::addressof(CPTR(const Context, ctx)->document())); }
RDAssembler* RDContext_GetAssembler(const RDContext* ctx) { return CPTR(RDAssembler, CPTR(const Context, ctx)->assembler()); }
RDLoader* RDContext_GetLoader(const RDContext* ctx) { return CPTR(RDLoader, CPTR(const Context, ctx)->loader()); }
RDBuffer* RDContext_GetBuffer(const RDContext* ctx) { return CPTR(RDBuffer, CPTR(const Context, ctx)->buffer()); }
size_t RDContext_GetBufferSize(const RDContext* ctx) { return CPTR(const Context, ctx)->buffer()->size(); }
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
void RDContext_DisableAnalyzer(RDContext* ctx, const char* analyzerid) { if(analyzerid) CPTR(Context, ctx)->disableAnalyzer(analyzerid); }
void RDContext_DisassembleBlock(RDContext* ctx, const RDBlock* block) { CPTR(Context, ctx)->disassembleBlock(block); }
void RDContext_DisassembleAt(RDContext* ctx, rd_address address) { CPTR(Context, ctx)->disassembleAt(address); }
void RDContext_Disassemble(RDContext* ctx) { CPTR(Context, ctx)->disassemble(); }
bool RDContext_IsAnalyzerSelected(const RDContext* ctx, const RDAnalyzer* analyzer) { return analyzer ? CPTR(const Context, ctx)->isAnalyzerSelected(CPTR(const Analyzer, analyzer)) : false; }
bool RDContext_ExecuteCommand(const RDContext* ctx, const char* cmd, const RDArguments* a) { return CPTR(const Context, ctx)->executeCommand(cmd, a); }
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
const char* RDContext_FunctionHexDump(const RDContext* ctx, rd_address address, rd_address* resaddress) { return CPTR(const Context, ctx)->disassembler()->getFunctionHexDump(address, resaddress); }
u8* RDContext_GetBufferData(RDContext* ctx) { return CPTR(Context, ctx)->buffer()->data(); }
bool RDContext_GetView(const RDContext* ctx, rd_address address, size_t size, RDBufferView* view) { return CPTR(const Context, ctx)->document()->getView(address, size, view); }
void RDContext_Enqueue(RDContext* ctx, rd_address address) { CPTR(Context, ctx)->disassembler()->enqueue(address); }

const char* RD_FromWString(const char16_t* s, size_t* len)
{
    static std::string res;

    if(!s) return nullptr;
    if(!len || (len && (*len == RD_NVAL))) res = Utils::toString(s);
    else res = Utils::toString(std::u16string(s, *len));

    if(len) *len = res.size();
    return res.c_str();
}

const char* RD_HexDump(const RDContext* ctx, rd_address address, size_t size)
{
    static std::string s;
    s = CPTR(const Context, ctx)->document()->getHexDump(address, size);
    return s.empty() ? nullptr : s.c_str();
}

const char* RD_ReadString(const RDContext* ctx, rd_address address, size_t* len) { return CPTR(const Context, ctx)->document()->readString(address, len); }
const char16_t* RD_ReadWString(const RDContext* ctx, rd_address address, size_t* len) { return CPTR(const Context, ctx)->document()->readWString(address, len); }
u8* RD_FilePointer(const RDContext* ctx, rd_offset offset) { return CPTR(const Context, ctx)->document()->filepointer(offset); }
u8* RD_AddrPointer(const RDContext* ctx, rd_address address) { return CPTR(const Context, ctx)->document()->addrpointer(address); }
u8* RD_Pointer(const RDContext* ctx, rd_offset offset) { return CPTR(const Context, ctx)->document()->offspointer(offset);  }
RDLocation RD_FileOffset(const RDContext* ctx, const void* ptr) { return CPTR(const Context, ctx)->document()->fileoffset(ptr); }
RDLocation RD_AddressOf(const RDContext* ctx, const void* ptr) { return CPTR(const Context, ctx)->document()->addressof(ptr); }
RDLocation RD_Offset(const RDContext* ctx, rd_address address) { return CPTR(const Context, ctx)->document()->offset(address); }
RDLocation RD_Address(const RDContext* ctx, rd_offset offset) { return CPTR(const Context, ctx)->document()->address(offset); }
bool RD_IsAddress(const RDContext* ctx, rd_address address) { return CPTR(const Context, ctx)->document()->isAddress(address); }

const char* RD_MakeLabel(rd_address address, const char* prefix)
{
    static std::string s;
    s = Document::makeLabel(address, prefix ? prefix : std::string());
    return s.c_str();
}

bool RDContext_Save(RDContext* ctx, const char* filepath)
{
    if(!filepath) return false;

    ContextState cs(CPTR(Context, ctx));
    return cs.save(filepath);
}

bool RDContext_Load(RDContext* ctx, const char* filepath)
{
    if(!filepath) return false;

    ContextState cs(CPTR(Context, ctx));
    return cs.load(filepath);
}

const char* RD_Quoted(const char* s)
{
    if(!s) return nullptr;

    static std::string res;
    res = Utils::quoted(s);
    return res.c_str();
}

const char* RD_QuotedSingle(const char* s)
{
    if(!s) return nullptr;

    static std::string res;
    res = Utils::quotedSingle(s);
    return res.c_str();
}

void RDContext_SetAddressAssembler(RDContext* ctx, rd_address address, const char* assembler)
{
    if(assembler) CPTR(Context, ctx)->document()->setAddressAssembler(address, assembler);
}

const char* RDContext_GetAddressAssembler(const RDContext* ctx, rd_address address)
{
    static std::string assembler;

    auto optassembler = CPTR(const Context, ctx)->document()->getAddressAssembler(address);
    if(optassembler) assembler = *optassembler;
    else return nullptr;

    return assembler.c_str();
}

const char* RDContext_GetDefaultAssembler(const RDContext* ctx)
{
    static std::string assembler;

    auto optassembler = CPTR(const Context, ctx)->addressDatabase()->defaultAssembler();
    if(!optassembler) return nullptr;

    assembler = *optassembler;
    return assembler.c_str();
}
