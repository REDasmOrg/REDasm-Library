#pragma once

#include "macros.h"
#include "types.h"

typedef struct RDUIOptions {
    const char* text;
    bool selected;
} RDUIOptions;

typedef void (*Callback_Message)(const char* title, const char* text);
typedef bool (*Callback_Confirm)(const char* title, const char* text);
typedef int (*Callback_GetItem)(const char* title, const char* text, const RDUIOptions* options, size_t c);
typedef bool (*Callback_GetChecked)(const char* title, const char* text, RDUIOptions* options, size_t c);
typedef bool (*Callback_GetText)(const char* title, const char* text, char* outchar, size_t* size);
typedef bool (*Callback_GetDouble)(const char* title, const char* text, double* outval);
typedef bool (*Callback_GetSigned)(const char* title, const char* text, intptr_t* outval);
typedef bool (*Callback_GetUnsigned)(const char* title, const char* text, uintptr_t* outval);

typedef struct RDUI {
    Callback_Message message;
    Callback_Confirm confirm;
    Callback_GetChecked getchecked;
    Callback_GetItem getitem;
    Callback_GetText gettext;
    Callback_GetDouble getdouble;
    Callback_GetSigned getsigned;
    Callback_GetUnsigned getunsigned;
} RDUI;

RD_API_EXPORT void RD_SetUI(const RDUI* rdui);
RD_API_EXPORT void RDUI_Message(const char* title, const char* text);
RD_API_EXPORT bool RDUI_Confirm(const char* title, const char* text);
RD_API_EXPORT int RDUI_GetItem(const char* title, const char* text, const RDUIOptions* options, size_t c);
RD_API_EXPORT bool RDUI_GetChecked(const char* title, const char* text, RDUIOptions* options, size_t c);
RD_API_EXPORT bool RDUI_GetText(const char* title, const char* text, char* outchar, size_t* size);
RD_API_EXPORT bool RDUI_GetDouble(const char* title, const char* text, double* outval);
RD_API_EXPORT bool RDUI_GetSigned(const char* title, const char* text, intptr_t* outval);
RD_API_EXPORT bool RDUI_GetUnsigned(const char* title, const char* text, uintptr_t* outval);
