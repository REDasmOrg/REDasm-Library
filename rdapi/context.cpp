#include "context.h"
#include <rdcore/disassembler.h>
#include <rdcore/context.h>

RDContext* RD_GetContext(void) { return CPTR(RDContext, rd_ctx); }
RDDisassembler* RD_GetDisassembler() { return CPTR(RDDisassembler, rd_ctx->disassembler()); }
size_t RD_ProblemsCount(void) { return rd_ctx->problemsCount(); }
void RD_InitContext(void) { rd_ctx->init(); }
void RD_SetTempPath(const char* tmppath) { rd_ctx->setTempPath(tmppath); }
void RD_SetRuntimePath(const char* rntpath) { rd_ctx->setRuntimePath(rntpath); }
void RD_SetLogCallback(RD_LogCallback callback, void* userdata) { rd_ctx->setLogCallback(callback, userdata); }
void RD_SetStatusCallback(RD_StatusCallback callback, void* userdata) { rd_ctx->setStatusCallback(callback, userdata); }
void RD_SetProgressCallback(RD_ProgressCallback callback, void* userdata) { rd_ctx->setProgressCallback(callback, userdata); }
void RD_AddPluginPath(const char* pluginpath) { rd_ctx->addPluginPath(pluginpath); }
void RD_AddDatabasePath(const char* dbpath) { rd_ctx->addDatabasePath(dbpath); }
void RD_GetProblems(RD_ProblemCallback callback, void* userdata) { rd_ctx->getProblems(callback, userdata); }
void RD_GetDatabasePaths(RD_PathCallback callback, void* userdata) { rd_ctx->getDatabasePaths(callback, userdata); }
void RD_GetPluginPaths(RD_PathCallback callback, void* userdata) { rd_ctx->getPluginPaths(callback, userdata); }
void RD_Log(const char* s) { rd_ctx->log(s); }
void RD_StatusAddress(const char* s, rd_address address) { rd_ctx->statusAddress(s, address); }
void RD_Status(const char* s) { rd_ctx->status(s); }
void RD_Problem(const char* s) { rd_ctx->problem(s); }
void RD_SetIgnoreProblems(bool ignore) { rd_ctx->setIgnoreProblems(ignore);  }
bool RD_HasProblems(void) { return rd_ctx->hasProblems(); }
bool RD_IsBusy(void) { return rd_disasm->busy(); }

void RD_InitContextFlags(rd_flag flags) { rd_ctx->initFlags(flags); }
void RD_SetContextFlags(rd_flag flags, bool set) { rd_ctx->setFlags(flags, set); }
bool RD_ContextHasFlags(rd_flag flags) { return rd_ctx->hasFlags(flags); }
rd_flag RD_GetContextFlags(void) { return rd_ctx->flags(); }
const char* RD_RuntimePath(void) { return rd_ctx->runtimePath(); }
const char* RD_TempPath(void) { return rd_ctx->tempPath(); }
