#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "events.h"
#include "types.h"

DECLARE_HANDLE(RDContext);

enum RDContextFlags {
    ContextFlags_None     = 0,
    ContextFlags_ShowRDIL = (1 << 0),
};

struct RDLoaderRequest;
struct RDEntryLoader;
struct RDEntryAssembler;
struct RDDisassembler;
struct RDNet;
struct RDDocument;
struct RDAssembler;
struct RDLoader;
struct RDBuffer;

typedef void (*RD_ProblemCallback)(const char* s, void* userdata);
typedef void (*Callback_AssemblerEntry)(const struct RDEntryAssembler* entry, void* userdata);
typedef void (*Callback_LoaderEntry)(const struct RDEntryLoader* entry, void* userdata);
typedef void (*Callback_Analyzer)(const struct RDAnalyzer* analyzer, void* userdata);

RD_API_EXPORT RDContext* RDContext_Create();
RD_API_EXPORT const RDEntryAssembler* RDContext_FindAssemblerEntry(const RDContext* ctx, const RDEntryLoader* entryloader);
RD_API_EXPORT RDDisassembler* RDContext_BuildDisassembler(RDContext* ctx, const RDLoaderRequest* req, const RDEntryLoader* entryloader, const RDEntryAssembler* entryassembler);
RD_API_EXPORT const RDNet* RDContext_GetNet(const RDContext* ctx);
RD_API_EXPORT RDDocument* RDContext_GetDocument(const RDContext* ctx);
RD_API_EXPORT RDDisassembler* RDContext_GetDisassembler(const RDContext* ctx);
RD_API_EXPORT RDAssembler* RDContext_GetAssembler(const RDContext* ctx);
RD_API_EXPORT RDLoader* RDContext_GetLoader(const RDContext* ctx);
RD_API_EXPORT RDBuffer* RDContext_GetBuffer(const RDContext* ctx);
RD_API_EXPORT size_t RDContext_GetBits(const RDContext* ctx);
RD_API_EXPORT size_t RDContext_GetAddressWidth(const RDContext* ctx);
RD_API_EXPORT void RDContext_SetUserData(RDContext* ctx, const char* s, uintptr_t userdata);
RD_API_EXPORT uintptr_t RDContext_GetUserData(const RDContext* ctx, const char* s);
RD_API_EXPORT bool RDContext_IsBusy(const RDContext* ctx);
RD_API_EXPORT void RDContext_FindLoaderEntries(RDContext* ctx, const RDLoaderRequest* loadrequest, Callback_LoaderEntry callback, void* userdata);
RD_API_EXPORT void RDContext_FindAssemblerEntries(const RDContext* ctx, Callback_AssemblerEntry callback, void* userdata);
RD_API_EXPORT void RDContext_GetAnalyzers(const RDContext* ctx, Callback_Analyzer callback, void* userdata);
RD_API_EXPORT void RDContext_SelectAnalyzer(RDContext* ctx, const RDAnalyzer* analyzer, bool select);
RD_API_EXPORT void RDContext_Subscribe(RDContext* ctx, void* owner, Callback_Event listener, void* userdata);
RD_API_EXPORT void RDContext_Unsubscribe(RDContext* ctx, void* owner);
RD_API_EXPORT void RDContext_DisassembleAt(RDContext* ctx, rd_address address);
RD_API_EXPORT void RDContext_Disassemble(RDContext* ctx);
RD_API_EXPORT void RDContext_GetProblems(const RDContext* ctx, RD_ProblemCallback callback, void* userdata);
RD_API_EXPORT void RDContext_AddProblem(RDContext* ctx, const char* s);
RD_API_EXPORT void RDContext_SetIgnoreProblems(RDContext* ctx, bool ignore);
RD_API_EXPORT bool RDContext_HasProblems(const RDContext* ctx);

// vvv OLD vvv
RD_API_EXPORT void RD_SetContextFlags(rd_flag flags, bool set);
RD_API_EXPORT rd_flag RD_GetContextFlags(void);
RD_API_EXPORT bool RD_ContextHasFlags(rd_flag flags);

#ifdef __cplusplus
  #include <string>
  #define rdcontext_addproblem(ctx, s) RDContext_AddProblem(ctx, std::string(s).c_str())
#endif
