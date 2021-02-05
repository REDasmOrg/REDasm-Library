#pragma once

#include "types.h"
#include "macros.h"
#include "document/document.h"

#define RD_EVENTARGS_BASE \
    event_id_t id; \
    void* sender; \
    void* owner; \
    RD_USERDATA_FIELD;

typedef u32 event_id_t;

enum RDEvents {
    Event_None = 0,

    // General Events
    Event_BusyChanged,
    Event_Error,

    // Context Events
    Event_ContextFlagsChanged,
    Event_ContextSurfaceChanged,
    Event_ContextFree,

    // Document Events
    Event_DocumentChanged,

    // Surface Events
    Event_SurfaceUpdated,
    Event_SurfaceScrollChanged,
    Event_SurfaceHistoryChanged,
    Event_SurfacePositionChanged,
};

struct RDSurfacePos;

typedef struct RDEventArgs {
    RD_EVENTARGS_BASE
} RDEventArgs;

typedef struct RDErrorEventArgs {
    RD_EVENTARGS_BASE
    const char* message;
} RDErrorEventArgs;

typedef struct RDDocumentEventArgs {
    RD_EVENTARGS_BASE

    RDDocumentAction action;
    RDDocumentItem item;
} RDDocumentEventArgs;

typedef struct RDSurfaceEventArgs {
    RD_EVENTARGS_BASE

    const RDSurfacePos* position;
    const RDSurfacePos* selection;
    RDDocumentItem item;
} RDSurfaceEventArgs;
