#pragma once

#include "../macros.h"
#include "../types.h"
#include "../plugin.h"

struct RDAnalyzerPlugin;
struct RDDisassembler;
struct RDAssembler;
struct RDLoader;

enum RDAnalyzerFlags {
    AnalyzerFlags_None         = 0,
    AnalyzerFlags_Selected     = (1 << 0),
    AnalyzerFlags_RunOnce      = (1 << 1),
    AnalyzerFlags_Experimental = (1 << 2),
};

typedef void (*Callback_AnalyzerPlugin)(const struct RDAnalyzerPlugin* plugin, void* userdata);
typedef bool (*Callback_AnalyzerIsEnabled)(const struct RDAnalyzerPlugin* plugin, const RDLoader* loader, const RDAssembler* assembler);
typedef void (*Callback_AnalyzerExecute)(const struct RDAnalyzerPlugin* plugin, RDDisassembler* disassembler);

typedef struct RDAnalyzerPlugin {
    RD_PLUGIN_HEADER

    u64 priority;
    const char* description;
    rd_flag flags;
    Callback_AnalyzerIsEnabled isenabled;
    Callback_AnalyzerExecute execute;
} RDAnalyzerPlugin;

RD_API_EXPORT void RD_GetEnabledAnalyzers(Callback_AnalyzerPlugin callback, void* userdata);
RD_API_EXPORT void RDAnalyzer_Select(const RDAnalyzerPlugin* panalyzer, bool selected);
RD_API_EXPORT bool RDAnalyzer_Register(RDAnalyzerPlugin* plugin);
