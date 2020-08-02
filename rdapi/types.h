#pragma once

#include <stdint.h>
#include <stddef.h>
#include "macros.h"

#define DEFAULT_SYMBOL_NAME_SIZE 32
#define DEFAULT_FULL_NAME_SIZE   64
#define DEFAULT_NAME_SIZE        32
#define DEFAULT_CONTAINER_SIZE   32

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

typedef u64 rd_size;
typedef rd_size rd_location;
typedef rd_location rd_address;
typedef rd_location rd_offset;

typedef u64 rd_instruction_id;
typedef u64 rd_register_id;

typedef u16 rd_type;
typedef u16 rd_flag;

#ifdef __cplusplus
    #define RD_NPOS static_cast<size_t>(-1)
#else
    #define RD_NPOS (size_t)(-1)
#endif

#ifdef __cplusplus
    #define RD_NREG static_cast<rd_register_id>(-1)
#else
    #define RD_NREG (rd_register_id)(-1)
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
        rd_location value;
        rd_offset offset;
        rd_address address;
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
    rd_offset offset;
    rd_offset endoffset;
    rd_address address;
    rd_address endaddress;
    rd_flag flags;
    size_t coveragebytes;

    RD_USERDATA_FIELD
} RDSegment;
#pragma pack(pop)

enum RDOperandType {
    OperandType_Void,
    OperandType_Constant,               // Simple constant
    OperandType_Register,               // Register
    OperandType_Immediate,              // Immediate Value
    OperandType_Memory,                 // Direct Memory Pointer
    OperandType_Displacement,           // Indirect Memory Pointer
    OperandType_Custom        = 0x1000, // Custom Operand
};

enum RDOperandFlags {
    OperandFlags_None    = 0,
    OperandFlags_Virtual = (1 << 0)
};

#pragma pack(push, 1)
typedef struct RDOperand {
    rd_type type;
    rd_flag flags;

    union {
        rd_register_id reg;
        rd_register_id reg1;
        rd_register_id base;
    };

    union {
        rd_register_id index;
        rd_register_id reg2;
    };

    union {
        s64 scale;
        rd_register_id reg3;
    };

    union {
        s64 s_value;
        s64 displacement;

        u64 u_value;
        rd_address address;
        rd_offset offset;
        rd_register_id reg4;
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
    rd_instruction_id id;  // Implementation Specific

    rd_address address;
    char mnemonic[DEFAULT_NAME_SIZE];
    rd_type type;
    rd_flag flags;
    u32 size;
    u32 operandscount;
    RDOperand operands[DEFAULT_CONTAINER_SIZE];

    RD_USERDATA_FIELD
} RDInstruction;
#pragma pack(pop)

RD_API_EXPORT size_t RDSegment_RawSize(const RDSegment* s);
RD_API_EXPORT size_t RDSegment_Size(const RDSegment* s);
RD_API_EXPORT bool RDSegment_ContainsAddress(const RDSegment* s, rd_address address);
RD_API_EXPORT bool RDSegment_ContainsOffset(const RDSegment* s, rd_offset offset);
RD_API_EXPORT rd_address RDInstruction_NextAddress(const RDInstruction* instruction);
RD_API_EXPORT RDOperand* RDInstruction_PushOperand(RDInstruction* instruction, rd_type type);
RD_API_EXPORT RDOperand* RDInstruction_FirstOperand(RDInstruction* instruction);
RD_API_EXPORT RDOperand* RDInstruction_LastOperand(RDInstruction* instruction);
RD_API_EXPORT void RDInstruction_ClearOperands(RDInstruction* instruction);
RD_API_EXPORT void RDInstruction_PopOperand(RDInstruction* instruction, size_t idx);
RD_API_EXPORT void RDInstruction_SetMnemonic(RDInstruction* instruction, const char* mnemonic);
RD_API_EXPORT bool RDInstruction_MnemonicIs(const RDInstruction* instruction, const char* mnemonic);
