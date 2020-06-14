#pragma once

#include "types.h"
#include "macros.h"
#include "document/document.h"

#define RD_EVENTARGS_BASE \
    event_id_t eventid; \
    void* sender; \
    void* owner; \
    RD_USERDATA_FIELD;

typedef u32 event_id_t;

enum RDEvents {
    Event_None = 0,

    // General Events
    Event_BusyChanged,
    Event_Error,

    // Document Events
    Event_DocumentChanged,

    // Cursor Events
    Event_CursorPositionChanged,
    Event_CursorStackChanged,

    // User defined Events
    Event_UserFirst = (1u << 31),
};

struct RDCursor;
struct RDCursorPos;

typedef struct RDEventArgs {
    RD_EVENTARGS_BASE
} RDEventArgs;

typedef struct RDErrorEventArgs {
    RD_EVENTARGS_BASE
    const char* message;
} RDErrorEventArgs;

typedef struct RDDocumentEventArgs {
    RD_EVENTARGS_BASE

    rd_type action;
    size_t index;
    RDDocumentItem item;
} RDDocumentEventArgs;

typedef struct RDCursorEventArgs {
    RD_EVENTARGS_BASE

    const RDCursorPos* position;
    const RDCursorPos* selection;
} RDCursorEventArgs;

typedef void (*Callback_Event)(const RDEventArgs* e);

RD_API_EXPORT void RDEvent_Subscribe(void* owner, Callback_Event eventcb, void* userdata);
RD_API_EXPORT void RDEvent_Unsubscribe(void* owner);
