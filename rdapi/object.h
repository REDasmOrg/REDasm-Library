#pragma once

#include "macros.h"
#include "types.h"

DECLARE_HANDLE(RDObject);

RD_API_EXPORT void RD_Free(void* obj);

#ifdef __cplusplus
#include <memory>

struct RDObjectDeleter {
    void operator()(void* ptr) const { RD_Free(ptr); }
};

template<typename T> using rd_ptr = std::unique_ptr<T, RDObjectDeleter>;
#endif
