#pragma once

#include "../macros.h"
#include "../types.h"
#include "types.h"

enum RDDatabaseValueTypes {
    DatabaseValueType_Null = 0,
    DatabaseValueType_UInt,
    DatabaseValueType_Int,
    DatabaseValueType_Float,
    DatabaseValueType_Bool,
    DatabaseValueType_String,
    DatabaseValueType_Object,
    DatabaseValueType_Array,
};

typedef struct RDDatabaseValue {
    rd_type type;

    union {
        u64 u;
        s64 i;
        double f;
        bool b;
        const char* s;
        const char* obj;
        const char* arr;
    };
} RDDatabaseValue;

DECLARE_HANDLE(RDDatabase);

RD_API_EXPORT size_t RDDatabase_CompileFile(const char* filepath, const u8** compiled);
RD_API_EXPORT size_t RDDatabase_DecompileFile(const char* filepath, const u8** decompiled);

RD_API_EXPORT RDDatabase* RDDatabase_Open(const char* dbname);
RD_API_EXPORT RDDatabase* RDDatabase_Create(const char* dbname);
RD_API_EXPORT const char* RDDatabase_GetName(const RDDatabase* db);
RD_API_EXPORT bool RDDatabase_Query(const RDDatabase* db, const char* q, RDDatabaseValue* dbvalue);
