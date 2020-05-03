#include "context.h"
#include <rdcore/disassembler.h>
#include <rdcore/context.h>

RDContext* RD_GetContext(void) { return CPTR(RDContext, rd_ctx); }
void RD_InitContext(void) { rd_ctx->init(); }
void RD_SetTempPath(const char* tmppath) { rd_ctx->setTempPath(tmppath); }
void RD_SetRuntimePath(const char* rntpath) { rd_ctx->setRuntimePath(rntpath); }
void RD_SetLogCallback(RD_LogCallback callback, void* userdata) { rd_ctx->setLogCallback(callback, userdata); }
void RD_SetStatusCallback(RD_StatusCallback callback, void* userdata) { rd_ctx->setStatusCallback(callback, userdata); }
void RD_SetProgressCallback(RD_ProgressCallback callback, void* userdata) { rd_ctx->setProgressCallback(callback, userdata); }
void RD_AddPluginPath(const char* pluginpath) { rd_ctx->addPluginPath(pluginpath); }
void RD_AddDatabasePath(const char* dbpath) { rd_ctx->addDatabasePath(dbpath); }
void RD_GetProblems(RD_ProblemCallback callback, void* userdata) { rd_ctx->getProblems(callback, userdata); }
void RD_Log(const char* s) { rd_ctx->log(s); }
void RD_Status(const char* s) { rd_ctx->status(s); }
void RD_Problem(const char* s) { rd_ctx->problem(s); }
void RD_SetIgnoreProblems(bool ignore) { rd_ctx->setIgnoreProblems(ignore);  }
void RD_SetSync(bool sync) { rd_ctx->sync(sync); }
bool RD_IsSync(void) { return rd_ctx->sync(); }
bool RD_IsBusy() { return rd_disasm->busy(); }

RDDisassembler* RD_GetDisassembler(void) { return CPTR(RDDisassembler, rd_disasm); }
RDDocument* RD_GetDocument() { return CPTR(RDDocument, std::addressof(rd_doc)); }
void RD_SetContextFlags(flag_t flags) { rd_ctx->setFlags(flags); }
flag_t RD_GetContextFlags() { return rd_ctx->flags(); }
const char* RD_RuntimePath() { return rd_ctx->runtimePath(); }
const char* RD_TempPath() { return rd_ctx->tempPath(); }
