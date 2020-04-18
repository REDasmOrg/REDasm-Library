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
typedef uintptr_t tag_t;
typedef u32 object_id_t;
typedef object_id_t argument_t;

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

typedef struct RDLocation {
    union {
        location_t value;
        offset_t offset;
        address_t address;
    };

    bool valid;
} RDLocation;

enum RDSegmentType {
    SegmentType_None,
    SegmentType_Code = (1 << 0),
    SegmentType_Data = (1 << 1),
    SegmentType_Bss  = (1 << 2),

    SegmentType_CodeData  = SegmentType_Code | SegmentType_Data
};

typedef struct RDSegment {
    char name[DEFAULT_NAME_SIZE];
    offset_t offset, endoffset;
    address_t address, endaddress;
    type_t type;
    size_t coveragebytes;
    size_t itemscount;
} RDSegment;

enum RDOperandType {
    OperandType_None,
    OperandType_Constant,     // Simple constant
    OperandType_Register,     // Register
    OperandType_Immediate,    // Immediate Value
    OperandType_Memory,       // Direct Memory Pointer
    OperandType_Displacement, // Indirect Memory Pointer
};

enum RDOperandFlags {
    OperandFlags_None,
    OperandFlags_Local    = (1 << 1),  // Local Variable
    OperandFlags_Argument = (1 << 2),  // Function Argument
    OperandFlags_Target   = (1 << 3),  // Branch destination
};

typedef struct RDOperand {
    type_t type;
    flag_t flags;
    u32 size, pos, locindex;

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
    };
} RDOperand;

enum RDInstructionType {
    InstructionType_None,
    InstructionType_Invalid,
    InstructionType_Stop,
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
    InstructionType_Store
};

enum RDInstructionFlags {
    InstructionFlags_None,
    InstructionFlags_Weak        = (1 << 1),
    InstructionFlags_Conditional = (1 << 2),
    InstructionFlags_Privileged  = (1 << 3),
};

typedef void(*RD_FreeCallback)(uintptr_t);

typedef struct RDInstruction {
    instruction_id_t id;  // Implementation Specific
    uintptr_t userdata;   // Implementation Specific
    RD_FreeCallback free; // Implementation Specific

    address_t address;
    char mnemonic[DEFAULT_NAME_SIZE];
    type_t type;
    flag_t flags;
    u32 size;
    //size_t targetscount;
    //address_t targets[DEFAULT_CONTAINER_SIZE];
    u32 operandscount;
    RDOperand operands[DEFAULT_CONTAINER_SIZE];
} RDInstruction;

RD_API_EXTERN_C size_t RDSegment_RawSize(const RDSegment* s);
RD_API_EXTERN_C size_t RDSegment_Size(const RDSegment* s);
RD_API_EXTERN_C bool RDInstruction_PushOperand(RDInstruction* instruction, RDOperand* op);
RD_API_EXTERN_C void RDInstruction_SetMnemonic(RDInstruction* instruction, const char* mnemonic);
