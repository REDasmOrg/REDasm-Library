#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "object.h"
#include "macros.h"

DECLARE_HANDLE(RDContext);

enum RDContextFlags {
    ContextFlag_None              = 0,
    ContextFlag_DisableUnexplored = (1 << 0),
    ContextFlag_DisableAnalyzer   = (1 << 1),
    ContextFlag_DisableSignature  = (1 << 2),
    ContextFlag_DisableCFG        = (1 << 3),
};

struct RDDisassembler;
struct RDDocument;
struct RDLoader;

typedef void (*RD_LogCallback)(const char* s, void* userdata);
typedef void (*RD_StatusCallback)(const char* s, void* userdata);
typedef void (*RD_ProgressCallback)(size_t pending, void* userdata);
typedef void (*RD_ProblemCallback)(const char* s, void* userdata);

// General
RD_API_EXPORT RDContext* RD_GetContext(void);
RD_API_EXPORT size_t RD_ProblemsCount(void);
RD_API_EXPORT void RD_InitContext(void);
RD_API_EXPORT void RD_SetRuntimePath(const char* rntpath);
RD_API_EXPORT void RD_SetTempPath(const char* rntpath);
RD_API_EXPORT void RD_SetLogCallback(RD_LogCallback callback, void* userdata);
RD_API_EXPORT void RD_SetStatusCallback(RD_StatusCallback callback, void* userdata);
RD_API_EXPORT void RD_SetProgressCallback(RD_ProgressCallback callback, void* userdata);
RD_API_EXPORT void RD_AddPluginPath(const char* pluginpath);
RD_API_EXPORT void RD_AddDatabasePath(const char* dbpath);
RD_API_EXPORT void RD_GetProblems(RD_ProblemCallback callback, void* userdata);
RD_API_EXPORT void RD_StatusAddress(const char* s, address_t address);
RD_API_EXPORT void RD_Status(const char* s);
RD_API_EXPORT void RD_Log(const char* s);
RD_API_EXPORT void RD_Problem(const char* s);
RD_API_EXPORT void RD_SetIgnoreProblems(bool ignore);
RD_API_EXPORT bool RD_HasProblems(void);
RD_API_EXPORT bool RD_IsBusy(void);

// Context
RD_API_EXPORT void RD_SetContextFlags(flag_t flags);
RD_API_EXPORT flag_t RD_GetContextFlags(void);
RD_API_EXPORT const char* RD_RuntimePath(void);
RD_API_EXPORT const char* RD_TempPath(void);
RD_API_EXPORT RDDisassembler* RD_GetDisassembler(void);
RD_API_EXPORT RDDocument* RD_GetDocument(void);

#ifdef __cplusplus
  #include <string>
  #define rd_log(s)                    RD_Log(std::string(s).c_str())
  #define rd_problem(s)                RD_Problem(std::string(s).c_str())
  #define rd_status(s)                 RD_Status(std::string(s).c_str())
  #define rd_statusaddress(s, address) RD_StatusAddress(std::string(s).c_str(), address)
#endif
