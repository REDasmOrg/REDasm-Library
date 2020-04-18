#pragma once

#include "types.h"
#include "macros.h"
#include "document/document.h"

#define RD_EVENTARGS_BASE \
    event_id_t eventid; \
    void* sender;

typedef u32 event_id_t;
typedef uintptr_t event_t;

enum RDEvents {
    Event_None = 0,

    // Standard Events
    Event_DisassemblerBusyChanged,

    // Document Events
    Event_DocumentBlockInserted,
    Event_DocumentBlockRemoved,
    Event_DocumentChanged,

    // Cursor Events
    Event_CursorPositionChanged,
    Event_CursorStackChanged,

    // User defined Events
    Event_UserFirst = (1u << 31),
};

struct RDBlock;
struct RDCursor;
struct RDCursorPos;

typedef struct RDEventArgs {
    RD_EVENTARGS_BASE
} RDEventArgs;

typedef struct RDDocumentEventArgs {
    RD_EVENTARGS_BASE

    type_t action;
    size_t index;
    RDDocumentItem item;
} RDDocumentEventArgs;

typedef struct RDCursorEventArgs {
    RD_EVENTARGS_BASE

    const RDCursorPos* position;
    const RDCursorPos* selection;
} RDCursorEventArgs;

typedef struct RDDocumentBlockEventArgs {
    RD_EVENTARGS_BASE

    RDBlock block;
} RDDocumentBlockEventArgs;

typedef void(*RD_EventCallback)(const RDEventArgs* e, void* userdata);

RD_API_EXPORT event_t RDEvent_Subscribe(event_id_t id, RD_EventCallback eventcb, void* userdata);
RD_API_EXPORT void RDEvent_Unsubscribe(event_t event);
RD_API_EXPORT void RDEvent_UnsubscribeAll();
