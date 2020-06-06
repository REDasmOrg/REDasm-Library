#pragma once

#include <stdint.h>
#include <stddef.h>
#include "macros.h"

#define DEFAULT_SYMBOL_NAME_SIZE 32
#define DEFAULT_NAME_SIZE        32
#define DEFAULT_CONTAINER_SIZE   10

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

typedef u64 location_t;
typedef location_t address_t;
typedef location_t offset_t;

typedef u64 instruction_id_t;
typedef s64 register_id_t;

typedef u16 type_t;
typedef u16 flag_t;

#ifdef __cplusplus
    #define RD_NPOS static_cast<size_t>(-1)
#else
    #define RD_NPOS (size_t)(-1)
#endif

#ifdef __cplusplus
    #define RD_NREG static_cast<register_id_t>(-1)
#else
    #define RD_NREG (register_id_t)(-1)
#endif

#define RD_USERDATA_FIELD \
    union { \
        void* userdata; \
        void* p_data; \
        intptr_t i_data; \
        uintptr_t u_data; \
        const char* s_data; \
    };

typedef struct RDLocation {
    union {
        location_t value;
        offset_t offset;
        address_t address;
    };

    bool valid;
} RDLocation;

enum RDSegmentFlags {
    SegmentFlags_None,
    SegmentFlags_Code = (1 << 0),
    SegmentFlags_Data = (1 << 1),
    SegmentFlags_Bss  = (1 << 2),

    SegmentFlags_CodeData  = SegmentFlags_Code | SegmentFlags_Data,
};

#pragma pack(push, 1)
typedef struct RDSegment {
    char name[DEFAULT_NAME_SIZE];
    offset_t offset;
    offset_t endoffset;
    address_t address;
    address_t endaddress;
    flag_t flags;
    size_t coveragebytes;
    size_t itemscount;
} RDSegment;
#pragma pack(pop)

enum RDOperandType {
    OperandType_Void,
    OperandType_Constant,     // Simple constant
    OperandType_Register,     // Register
    OperandType_Immediate,    // Immediate Value
    OperandType_Memory,       // Direct Memory Pointer
    OperandType_Displacement, // Indirect Memory Pointer
};

#pragma pack(push, 1)
typedef struct RDOperand {
    type_t type;
    u32 pos;

    union {
        register_id_t reg;
        register_id_t base;
    };

    register_id_t index;
    s64 scale;

    union {
        s64 s_value;
        s64 displacement;

        u64 u_value;
        address_t address;
        offset_t offset;
    };

    RD_USERDATA_FIELD
} RDOperand;
#pragma pack(pop)

enum RDInstructionType {
    InstructionType_None,
    InstructionType_Invalid,
    InstructionType_Ret,
    InstructionType_Nop,
    InstructionType_Jump,
    InstructionType_Call,
    InstructionType_Add,
    InstructionType_Sub,
    InstructionType_Mul,
    InstructionType_Div,
    InstructionType_Mod,
    InstructionType_Lsh,
    InstructionType_Rsh,
    InstructionType_And,
    InstructionType_Or,
    InstructionType_Xor,
    InstructionType_Not,
    InstructionType_Push,
    InstructionType_Pop,
    InstructionType_Compare,
    InstructionType_Load,
    InstructionType_Store,
};

enum RDInstructionFlags {
    InstructionFlags_None,
    InstructionFlags_Weak        = (1 << 1),
    InstructionFlags_Conditional = (1 << 2),
    InstructionFlags_Privileged  = (1 << 3),
    InstructionFlags_Stop        = (1 << 4),
};

#pragma pack(push, 1)
typedef struct RDInstruction {
    instruction_id_t id;  // Implementation Specific

    address_t address;
    char mnemonic[DEFAULT_NAME_SIZE];
    type_t type;
    flag_t flags;
    u32 size;
    u32 operandscount;
    RDOperand operands[DEFAULT_CONTAINER_SIZE];

    RD_USERDATA_FIELD
} RDInstruction;
#pragma pack(pop)

RD_API_EXPORT size_t RDSegment_RawSize(const RDSegment* s);
RD_API_EXPORT size_t RDSegment_Size(const RDSegment* s);
RD_API_EXPORT address_t RDInstruction_NextAddress(const RDInstruction* instruction);
RD_API_EXPORT RDOperand* RDInstruction_PushOperand(RDInstruction* instruction, type_t type);
RD_API_EXPORT void RDInstruction_ClearOperands(RDInstruction* instruction);
RD_API_EXPORT void RDInstruction_PopOperand(RDInstruction* instruction, size_t idx);
RD_API_EXPORT void RDInstruction_SetMnemonic(RDInstruction* instruction, const char* mnemonic);
RD_API_EXPORT bool RDInstruction_MnemonicIs(const RDInstruction* instruction, const char* mnemonic);
