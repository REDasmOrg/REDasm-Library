#pragma once

#include "../types.h"
#include "../macros.h"

enum RDBlockType {
    BlockType_Unexplored,
    BlockType_Data,
    BlockType_Code,
};

typedef struct RDBlock {
    address_t start, end; // [start, end]
    type_t type;
} RDBlock;

RD_API_EXPORT bool RDBlock_Contains(const RDBlock* b, address_t address);
RD_API_EXPORT size_t RDBlock_Empty(const RDBlock* b);
RD_API_EXPORT size_t RDBlock_Size(const RDBlock* b);
