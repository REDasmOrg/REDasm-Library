#include "ui.h"
#include <rdcore/config.h>
#include <rdcore/support/error.h>

#define RET_CALL_RDUI(uimethod, ...) \
    if(!rd_cfg->ui()) REDasmError("UI not set"); \
    if(!rd_cfg->ui()->uimethod) REDasmError("UI '" + std::string(#uimethod) + "' not set"); \
    return rd_cfg->ui()->uimethod(__VA_ARGS__);

void RDUI_Message(const char* title, const char* text) { RET_CALL_RDUI(message, title, text); }
bool RDUI_Confirm(const char* title, const char* text) { RET_CALL_RDUI(confirm, title, text); }
int RDUI_GetItem(const char* title, const char* text, const RDUIOptions* options, size_t c) { RET_CALL_RDUI(getitem, title, text, options, c); }
bool RDUI_GetChecked(const char* title, const char* text, RDUIOptions* options, size_t c) { RET_CALL_RDUI(getchecked, title, text, options, c); }
bool RDUI_GetText(const char* title, const char* text, char* outchar, size_t* size) { RET_CALL_RDUI(gettext, title, text, outchar, size); }
bool RDUI_GetDouble(const char* title, const char* text, double* outval) { RET_CALL_RDUI(getdouble, title, text, outval); }
bool RDUI_GetSigned(const char* title, const char* text, intptr_t* outval) { RET_CALL_RDUI(getsigned, title, text, outval); }
bool RDUI_GetUnsigned(const char* title, const char* text, uintptr_t* outval) { RET_CALL_RDUI(getunsigned, title, text, outval); }
