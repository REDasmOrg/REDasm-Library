#include "config.h"
#include <rdcore/config.h>

void RDConfig_SetUI(const RDUI* ui) { rd_cfg->setUI(ui); }
void RDConfig_SetRuntimePath(const char* rntpath) { rd_cfg->setRuntimePath(rntpath); }
void RDConfig_SetTempPath(const char* rntpath) { rd_cfg->setTempPath(rntpath); }
void RDConfig_AddPluginPath(const char* pluginpath) { rd_cfg->addPluginPath(pluginpath); }
void RDConfig_AddDatabasePath(const char* dbpath) { rd_cfg->addDatabasePath(dbpath); }
void RDConfig_GetDatabasePaths(RD_PathCallback callback, void* userdata) { rd_cfg->getDatabasePaths(callback, userdata); }
void RDConfig_GetPluginPaths(RD_PathCallback callback, void* userdata) { rd_cfg->getPluginPaths(callback, userdata); }
void RDConfig_SetLogCallback(RD_LogCallback callback, void* userdata) { rd_cfg->setLogCallback(callback, userdata); }
void RDConfig_SetStatusCallback(RD_StatusCallback callback, void* userdata) { rd_cfg->setStatusCallback(callback, userdata); }
void RDConfig_SetProgressCallback(RD_ProgressCallback callback, void* userdata) { rd_cfg->setProgressCallback(callback, userdata); }
void RDConfig_SetTheme(rd_type theme, const char* color) { rd_cfg->setTheme(theme, color); }
const char* RDConfig_GetTheme(rd_type theme) { return rd_cfg->theme(theme); }

const char* RDConfig_GetRuntimePath()
{
    static std::string s;
    s = rd_cfg->runtimePath();
    return s.c_str();
}

const char* RDConfig_GetTempPath()
{
    static std::string s;
    s = rd_cfg->tempPath();
    return s.c_str();
}

void RD_StatusAddress(const char* s, rd_address address) { rd_cfg->statusAddress(s, address); }
void RD_Status(const char* s) { rd_cfg->status(s); }
void RD_Log(const char* s) { rd_cfg->log(s); }
