#pragma once

#include "../types.h"
#include "../macros.h"

enum RDBlockType {
    BlockType_Unexplored,
    BlockType_Data,
    BlockType_Code,
};

typedef struct RDBlock { // [start, end]
    union {
        address_t start;
        address_t address;
    };

    address_t end;
    type_t type;
} RDBlock;

DECLARE_HANDLE(RDBlockContainer);

RD_API_EXPORT bool RDBlockContainer_Find(const RDBlockContainer* c, address_t address, RDBlock* block);
RD_API_EXPORT bool RDBlockContainer_Get(const RDBlockContainer* c, size_t index, RDBlock* block);
RD_API_EXPORT size_t RDBlockContainer_Index(const RDBlockContainer* c, const RDBlock* block);
RD_API_EXPORT size_t RDBlockContainer_Size(const RDBlockContainer* c);

RD_API_EXPORT bool RDBlock_Contains(const RDBlock* b, address_t address);
RD_API_EXPORT size_t RDBlock_Empty(const RDBlock* b);
RD_API_EXPORT size_t RDBlock_Size(const RDBlock* b);
