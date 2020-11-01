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

    // Context Events
    Event_ContextFlagsChanged,
    Event_ContextFree,

    // Document Events
    Event_DocumentChanged,

    // Cursor Events
    Event_CursorPositionChanged,

    // Surface Events
    Event_SurfaceCursorChanged,
    Event_SurfaceUpdated,
    Event_SurfaceChanged,

    // SurfacePath Events
    Event_SurfacePathChanged,

    // User defined Events
    Event_UserFirst = (1u << 31),
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

    rd_type action;
    RDDocumentItem item;
} RDDocumentEventArgs;

typedef struct RDSurfaceEventArgs {
    RD_EVENTARGS_BASE

    const RDSurfacePos* position;
    const RDSurfacePos* selection;
    RDDocumentItem item;
} RDSurfaceEventArgs;
