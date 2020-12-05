#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "object.h"
#include "types.h"

RD_HANDLE(RDContext);

enum RDCompilerABI {
    CompilerABI_Unknown = 0,
    CompilerABI_GNU,     // Gcc or CLang
    CompilerABI_MSVC,    // Visual C++
    CompilerABI_DotNET,     // Visual C++ .NET Mode
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
    ContextFlags_None       = 0,
    ContextFlags_NoDemangle = (1 << 0),
    ContextFlags_ShowRDIL   = (1 << 1),
};

struct RDLoaderRequest;
struct RDEntryLoader;
struct RDEntryAssembler;
struct RDNet;
struct RDDocument;
struct RDAssembler;
struct RDLoader;
struct RDBuffer;
struct RDSurface;
struct RDSymbol;

typedef void (*RD_ProblemCallback)(const char* s, void* userdata);
typedef void (*Callback_AssemblerEntry)(const struct RDEntryAssembler* entry, void* userdata);
typedef void (*Callback_LoaderEntry)(const struct RDEntryLoader* entry, void* userdata);
typedef void (*Callback_Analyzer)(const struct RDAnalyzer* analyzer, void* userdata);

RD_API_EXPORT RDContext* RDContext_Create();
RD_API_EXPORT RDLocation RDContext_GetEntryPoint(const RDContext* ctx);
RD_API_EXPORT RDLocation RDContext_GetFunctionStart(const RDContext* ctx, rd_address address);
RD_API_EXPORT RDLocation RDContext_Dereference(const RDContext* ctx, rd_address address);
RD_API_EXPORT const char* RDContext_FunctionHexDump(const RDContext* ctx, rd_address address, RDSymbol* symbol);
RD_API_EXPORT bool RDContext_MatchLoader(const RDContext* ctx, const char* q);
RD_API_EXPORT bool RDContext_MatchAssembler(const RDContext* ctx, const char* q);
RD_API_EXPORT bool RDContext_Bind(RDContext* ctx, const RDLoaderRequest* req, const RDEntryLoader* entryloader, const RDEntryAssembler* entryassembler);
RD_API_EXPORT bool RDContext_CreateFunction(RDContext* ctx, rd_address address, const char* name);
RD_API_EXPORT bool RDContext_ScheduleFunction(RDContext* ctx, rd_address address, const char* name);
RD_API_EXPORT void RDContext_Schedule(RDContext* ctx, rd_address address);
RD_API_EXPORT void RDContext_Enqueue(RDContext* ctx, rd_address address);
RD_API_EXPORT const RDEntryAssembler* RDContext_FindAssemblerEntry(const RDContext* ctx, const RDEntryLoader* entryloader);
RD_API_EXPORT const RDNet* RDContext_GetNet(const RDContext* ctx);
RD_API_EXPORT RDDocument* RDContext_GetDocument(const RDContext* ctx);
RD_API_EXPORT RDAssembler* RDContext_GetAssembler(const RDContext* ctx);
RD_API_EXPORT RDLoader* RDContext_GetLoader(const RDContext* ctx);
RD_API_EXPORT RDBuffer* RDContext_GetBuffer(const RDContext* ctx);
RD_API_EXPORT size_t RDContext_GetBits(const RDContext* ctx);
RD_API_EXPORT size_t RDContext_GetAddressWidth(const RDContext* ctx);
RD_API_EXPORT RDSurface* RDContext_GetActiveSurface(const RDContext* ctx);
RD_API_EXPORT void RDContext_SetUserData(RDContext* ctx, const char* s, uintptr_t userdata);
RD_API_EXPORT uintptr_t RDContext_GetUserData(const RDContext* ctx, const char* s);
RD_API_EXPORT bool RDContext_IsBusy(const RDContext* ctx);
RD_API_EXPORT void RDContext_FindLoaderEntries(RDContext* ctx, const RDLoaderRequest* loadrequest, Callback_LoaderEntry callback, void* userdata);
RD_API_EXPORT void RDContext_FindAssemblerEntries(const RDContext* ctx, Callback_AssemblerEntry callback, void* userdata);
RD_API_EXPORT void RDContext_GetAnalyzers(const RDContext* ctx, Callback_Analyzer callback, void* userdata);
RD_API_EXPORT void RDContext_SelectAnalyzer(RDContext* ctx, const RDAnalyzer* analyzer, bool select);
RD_API_EXPORT void RDContext_DisassembleAt(RDContext* ctx, rd_address address);
RD_API_EXPORT void RDContext_Disassemble(RDContext* ctx);
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

// Extra Functions
RD_API_EXPORT const char* RD_HexDump(const RDContext* ctx, rd_address address, size_t size);
RD_API_EXPORT const char* RD_ReadString(const RDContext* ctx, rd_address address, size_t* len);
RD_API_EXPORT const char16_t* RD_ReadWString(const RDContext* ctx, rd_address address, size_t* len);

#ifdef __cplusplus
  #include <string>
  #define rdcontext_addproblem(ctx, s) RDContext_AddProblem(ctx, std::string(s).c_str())
#endif
