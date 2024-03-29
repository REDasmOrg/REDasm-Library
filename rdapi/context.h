#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "object.h"
#include "types.h"

#define CONTEXT_STATE_EXT ".rds"

RD_HANDLE(RDContext);

enum RDCompilerABI {
    CompilerABI_Unknown = 0,
    CompilerABI_GNU,     // Gcc or CLang
    CompilerABI_MSVC,    // Visual C++
    CompilerABI_DotNET,  // Visual C++ (.NET Mode)
    CompilerABI_Borland, // Borland/Embarcadero
    CompilerABI_Go       // GoLang
};

enum RDCompilerCC {
    CompilerCC_Unknown = 0,
    CompilerCC_Cdecl,
    CompilerCC_Stdcall,
    CompilerCC_Pascal,
    CompilerCC_Fastcall,
    CompilerCC_Thiscall,
};

enum RDContextFlags {
    ContextFlags_None        = 0,
    ContextFlags_NoDemangle  = (1 << 0),
    ContextFlags_ShowRDIL    = (1 << 1),
    ContextFlags_NoCodeMerge = (1 << 2),
};

struct RDLoaderRequest;
struct RDEntryLoader;
struct RDBufferView;
struct RDEntryAssembler;
struct RDNet;
struct RDDocument;
struct RDAssembler;
struct RDLoader;
struct RDBuffer;
struct RDSurface;
struct RDDatabase;
struct RDBlock;
struct RDArguments;

typedef void (*RD_ProblemCallback)(const char* s, void* userdata);
typedef void (*Callback_AssemblerEntry)(const struct RDEntryAssembler* entry, void* userdata);
typedef void (*Callback_LoaderEntry)(const struct RDEntryLoader* entry, void* userdata);
typedef void (*Callback_Analyzer)(const struct RDAnalyzer* analyzer, void* userdata);

RD_API_EXPORT RDContext* RDContext_Create();
RD_API_EXPORT RDDatabase* RDContext_GetDatabase(const RDContext* ctx);
RD_API_EXPORT const char* RDContext_FunctionHexDump(const RDContext* ctx, rd_address address, rd_address* resaddress);
RD_API_EXPORT bool RDContext_MatchLoader(const RDContext* ctx, const char* q);
RD_API_EXPORT bool RDContext_MatchAssembler(const RDContext* ctx, const char* q);
RD_API_EXPORT bool RDContext_Bind(RDContext* ctx, const RDLoaderRequest* req, const RDEntryLoader* entryloader, const RDEntryAssembler* entryassembler);
RD_API_EXPORT void RDContext_Enqueue(RDContext* ctx, rd_address address);
RD_API_EXPORT const RDEntryAssembler* RDContext_FindAssemblerEntry(const RDContext* ctx, const RDEntryLoader* entryloader);
RD_API_EXPORT const RDNet* RDContext_GetNet(const RDContext* ctx);
RD_API_EXPORT RDDocument* RDContext_GetDocument(const RDContext* ctx);
RD_API_EXPORT RDAssembler* RDContext_GetAssembler(const RDContext* ctx);
RD_API_EXPORT RDLoader* RDContext_GetLoader(const RDContext* ctx);
RD_API_EXPORT RDBuffer* RDContext_GetBuffer(const RDContext* ctx);
RD_API_EXPORT u8* RDContext_GetBufferData(RDContext* ctx);
RD_API_EXPORT bool RDContext_GetView(const RDContext* ctx, rd_address address, size_t size, RDBufferView* view);
RD_API_EXPORT size_t RDContext_GetBufferSize(const RDContext* ctx);
RD_API_EXPORT size_t RDContext_GetBits(const RDContext* ctx);
RD_API_EXPORT size_t RDContext_GetAddressWidth(const RDContext* ctx);
RD_API_EXPORT size_t RDContext_GetMinString(const RDContext* ctx);
RD_API_EXPORT void RDContext_SetMinString(RDContext* ctx, size_t n);
RD_API_EXPORT RDSurface* RDContext_GetActiveSurface(const RDContext* ctx);
RD_API_EXPORT void RDContext_SetUserData(RDContext* ctx, const char* s, uintptr_t userdata);
RD_API_EXPORT uintptr_t RDContext_GetUserData(const RDContext* ctx, const char* s);
RD_API_EXPORT bool RDContext_IsBusy(const RDContext* ctx);
RD_API_EXPORT void RDContext_FindLoaderEntries(RDContext* ctx, const RDLoaderRequest* loadrequest, Callback_LoaderEntry callback, void* userdata);
RD_API_EXPORT void RDContext_FindAssemblerEntries(const RDContext* ctx, Callback_AssemblerEntry callback, void* userdata);
RD_API_EXPORT void RDContext_GetAnalyzers(const RDContext* ctx, Callback_Analyzer callback, void* userdata);
RD_API_EXPORT void RDContext_SelectAnalyzer(RDContext* ctx, const RDAnalyzer* analyzer, bool select);
RD_API_EXPORT void RDContext_DisableAnalyzer(RDContext* ctx, const char* analyzerid);
RD_API_EXPORT void RDContext_DisassembleBlock(RDContext* ctx, const RDBlock* block);
RD_API_EXPORT void RDContext_DisassembleAt(RDContext* ctx, rd_address address);
RD_API_EXPORT void RDContext_Disassemble(RDContext* ctx);
RD_API_EXPORT bool RDContext_IsAnalyzerSelected(const RDContext* ctx, const RDAnalyzer* analyzer);
RD_API_EXPORT bool RDContext_ExecuteCommand(const RDContext* ctx, const char* cmd, const struct RDArguments* a);
RD_API_EXPORT size_t RDContext_GetProblemsCount(const RDContext* ctx);
RD_API_EXPORT void RDContext_GetProblems(const RDContext* ctx, RD_ProblemCallback callback, void* userdata);
RD_API_EXPORT void RDContext_AddProblem(RDContext* ctx, const char* s);
RD_API_EXPORT void RDContext_SetIgnoreProblems(RDContext* ctx, bool ignore);
RD_API_EXPORT bool RDContext_HasProblems(const RDContext* ctx);
RD_API_EXPORT void RDContext_SetFlags(RDContext* ctx, rd_flag flags, bool set);
RD_API_EXPORT bool RDContext_HasFlag(const RDContext* ctx, rd_flag flag);
RD_API_EXPORT rd_flag RDContext_GetFlags(const RDContext* ctx);
RD_API_EXPORT void RDContext_SetABI(RDContext* ctx, rd_type t);
RD_API_EXPORT rd_type RDContext_GetABI(const RDContext* ctx);
RD_API_EXPORT void RDContext_SetCC(RDContext* ctx, rd_type t);
RD_API_EXPORT rd_type RDContext_GetCC(const RDContext* ctx);
RD_API_EXPORT void RDContext_SetAddressAssembler(RDContext* ctx, rd_address address, const char* assembler);
RD_API_EXPORT const char* RDContext_GetAddressAssembler(const RDContext* ctx, rd_address address);
RD_API_EXPORT const char* RDContext_GetDefaultAssembler(const RDContext* ctx);
RD_API_EXPORT bool RDContext_Load(RDContext* ctx, const char* filepath);
RD_API_EXPORT bool RDContext_Save(RDContext* ctx, const char* filepath);

// Extra Functions
RD_API_EXPORT const char* RD_Quoted(const char* s);
RD_API_EXPORT const char* RD_QuotedSingle(const char* s);
RD_API_EXPORT const char* RD_MakeLabel(rd_address address, const char* prefix);
RD_API_EXPORT const char* RD_FromWString(const char16_t* s, size_t* len);
RD_API_EXPORT const char* RD_HexDump(const RDContext* ctx, rd_address address, size_t size);
RD_API_EXPORT const char* RD_ReadString(const RDContext* ctx, rd_address address, size_t* len);
RD_API_EXPORT const char16_t* RD_ReadWString(const RDContext* ctx, rd_address address, size_t* len);
RD_API_EXPORT u8* RD_FilePointer(const RDContext* ctx, rd_offset offset);
RD_API_EXPORT u8* RD_AddrPointer(const RDContext* ctx, rd_address address);
RD_API_EXPORT u8* RD_Pointer(const RDContext* ctx, rd_offset offset);
RD_API_EXPORT RDLocation RD_FileOffset(const RDContext* ctx, const void* ptr);
RD_API_EXPORT RDLocation RD_AddressOf(const RDContext* ctx, const void* ptr);
RD_API_EXPORT RDLocation RD_Offset(const RDContext* ctx, rd_address address);
RD_API_EXPORT RDLocation RD_Address(const RDContext* ctx, rd_offset offset);
RD_API_EXPORT bool RD_IsAddress(const RDContext* ctx, rd_address address);

#ifdef __cplusplus
  #include <string>
  #define rdcontext_addproblem(ctx, s) RDContext_AddProblem(ctx, std::string(s).c_str())
  #define rd_makelabel(ctx, s) RD_MakeLabel(ctx, std::string(s).c_str())
  #define rd_quotedsingle(s) std::string(RD_QuotedSingle(s))
  #define rd_quoted(s) std::string(RD_Quoted(s))
#endif
