#pragma once

#include "types.h"

enum RDTheme {
    Theme_Default = 0,
    Theme_Foreground, Theme_Background, Theme_Seek, Theme_Comment, Theme_Meta,
    Theme_HighlightFg, Theme_HighlightBg,
    Theme_SelectionFg, Theme_SelectionBg,
    Theme_CursorFg, Theme_CursorBg,
    Theme_Segment, Theme_Function, Theme_Type,
    Theme_Address, Theme_Constant, Theme_Reg,
    Theme_String, Theme_Symbol, Theme_Data, Theme_Pointer, Theme_Imported,
    Theme_Nop, Theme_Ret, Theme_Call, Theme_Jump, Theme_JumpCond,
    Theme_EntryFg, Theme_EntryBg,
    Theme_GraphBg, Theme_GraphEdge, Theme_GraphEdgeTrue, Theme_GraphEdgeFalse, Theme_GraphEdgeLoop, Theme_GraphEdgeLoopCond
};

struct RDUI;
typedef void (*RD_PathCallback)(const char* s, void* userdata);
typedef void (*RD_LogCallback)(const char* s, void* userdata);
typedef void (*RD_StatusCallback)(const char* s, void* userdata);
typedef void (*RD_ProgressCallback)(size_t pending, void* userdata);

RD_API_EXPORT void RDConfig_SetUI(const RDUI* ui);
RD_API_EXPORT void RDConfig_SetRuntimePath(const char* rntpath);
RD_API_EXPORT void RDConfig_SetTempPath(const char* rntpath);
RD_API_EXPORT void RDConfig_AddPluginPath(const char* pluginpath);
RD_API_EXPORT void RDConfig_AddDatabasePath(const char* dbpath);
RD_API_EXPORT void RDConfig_GetDatabasePaths(RD_PathCallback callback, void* userdata);
RD_API_EXPORT void RDConfig_GetPluginPaths(RD_PathCallback callback, void* userdata);
RD_API_EXPORT void RDConfig_SetLogCallback(RD_LogCallback callback, void* userdata);
RD_API_EXPORT void RDConfig_SetStatusCallback(RD_StatusCallback callback, void* userdata);
RD_API_EXPORT void RDConfig_SetProgressCallback(RD_ProgressCallback callback, void* userdata);
RD_API_EXPORT void RDConfig_SetTheme(rd_type theme, const char* color);
RD_API_EXPORT const char* RDConfig_GetTheme(rd_type theme);
RD_API_EXPORT const char* RDConfig_GetRuntimePath(void);
RD_API_EXPORT const char* RDConfig_GetTempPath(void);

// Logging
RD_API_EXPORT void RD_StatusAddress(const char* s, rd_address address);
RD_API_EXPORT void RD_Status(const char* s);
RD_API_EXPORT void RD_Log(const char* s);

#ifdef __cplusplus
  #include <string>
  #define rd_log(s)                    RD_Log(std::string(s).c_str())
  #define rd_status(s)                 RD_Status(std::string(s).c_str())
  #define rd_statusaddress(s, address) RD_StatusAddress(std::string(s).c_str(), address)
#endif
