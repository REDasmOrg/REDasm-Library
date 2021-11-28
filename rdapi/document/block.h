#pragma once

#include "../types.h"
#include "../macros.h"

enum RDBlockType {
    BlockType_Unknown,
    BlockType_Data,
    BlockType_String,
    BlockType_Code,
};

enum RDAddressFlags {
    AddressFlags_None          = 0,

    AddressFlags_Explored      = (1 << 0),
    AddressFlags_Location      = (1 << 1),
    AddressFlags_Exported      = (1 << 2),
    AddressFlags_Imported      = (1 << 3),
    AddressFlags_Function      = (1 << 4),
    AddressFlags_AsciiString   = (1 << 5),
    AddressFlags_WideString    = (1 << 6),
    AddressFlags_Pointer       = (1 << 7),
    AddressFlags_NoReturn      = (1 << 8),
    AddressFlags_Type          = (1 << 9),
    AddressFlags_TypeField     = (1 << 10),
    AddressFlags_TypeEnd       = (1 << 11),
    AddressFlags_Length,

    AddressFlags_String        = AddressFlags_AsciiString | AddressFlags_WideString,
};

#pragma pack(push, 1)
typedef struct RDBlock { // [start, end)
    union {
        rd_address start;
        rd_address address;
    };

    rd_address end;
    rd_type type;

    union { // Internal use
      u16 info;
      u16 datainfo;
      u16 stringinfo;
      u16 codeinfo;
    };
} RDBlock;
#pragma pack(pop)

RD_API_EXPORT bool RDBlock_Contains(const RDBlock* b, rd_address address);
RD_API_EXPORT size_t RDBlock_Empty(const RDBlock* b);
RD_API_EXPORT size_t RDBlock_Size(const RDBlock* b);
