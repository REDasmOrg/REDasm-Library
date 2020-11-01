#pragma once

#include "../macros.h"
#include "../types.h"
#include "entry.h"

enum RDAnalyzerFlags {
    AnalyzerFlags_None         = 0,
    AnalyzerFlags_Selected     = (1 << 0),
    AnalyzerFlags_RunOnce      = (1 << 1),
    AnalyzerFlags_Experimental = (1 << 2),
};

DECLARE_HANDLE(RDAnalyzer);

typedef bool (*Callback_AnalyzerIsEnabled)(const RDContext* ctx);
typedef void (*Callback_AnalyzerExecute)(RDContext* ctx);

typedef struct RDEntryAnalyzer {
    RD_ENTRY_HEADER

    u32 order;
    const char* description;
    rd_flag flags;
    Callback_AnalyzerIsEnabled isenabled;
    Callback_AnalyzerExecute execute;
} RDEntryAnalyzer;

RD_API_EXPORT bool RDAnalyzer_Register(RDPluginModule* pm, const RDEntryAnalyzer* plugin);
RD_API_EXPORT bool RDAnalyzer_IsSelected(const RDAnalyzer* analyzer);
RD_API_EXPORT bool RDAnalyzer_IsExperimental(const RDAnalyzer* analyzer);
RD_API_EXPORT const char* RDAnalyzer_GetDescription(const RDAnalyzer* analyzer);
RD_API_EXPORT const char* RDAnalyzer_GetName(const RDAnalyzer* analyzer);
RD_API_EXPORT u32 RDAnalyzer_GetOrder(const RDAnalyzer* analyzer);
