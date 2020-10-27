#pragma once

#include "macros.h"
#include "types.h"

struct RDEventArgs;
typedef void (*Callback_Event)(const RDEventArgs* e);

typedef void RDObject;

RD_API_EXPORT void RDObject_Subscribe(RDObject* obj, void* owner, Callback_Event listener, void* userdata);
RD_API_EXPORT void RDObject_Unsubscribe(RDObject* obj, void* owner);
RD_API_EXPORT void RDObject_Free(RDObject* obj);

#ifdef __cplusplus
#include <memory>

struct RDObjectDeleter {
    void operator()(void* ptr) const { RDObject_Free(ptr); }
};

template<typename T> using rd_ptr = std::unique_ptr<T, RDObjectDeleter>;
#endif
