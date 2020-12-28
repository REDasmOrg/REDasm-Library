#pragma once

#include "../macros.h"
#include "../types.h"
#include "types.h"

#define DATABASE_RDB_EXT  ".rdb"
#define DATABASE_JSON_EXT ".json"

enum RDDatabaseValueTypes {
    DatabaseValueType_Null = 0,
    DatabaseValueType_UInt,
    DatabaseValueType_Int,
    DatabaseValueType_Float,
    DatabaseValueType_Bool,
    DatabaseValueType_String,
    DatabaseValueType_Object,
    DatabaseValueType_Array,
    DatabaseValueType_Type,
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
        const RDType* t;
    };
} RDDatabaseValue;

RD_HANDLE(RDDatabase);

RD_API_EXPORT size_t RDDatabase_CompileFile(const char* filepath, const u8** compiled);
RD_API_EXPORT size_t RDDatabase_DecompileFile(const char* filepath, const u8** decompiled);

RD_API_EXPORT RDDatabase* RDDatabase_Open(const char* dbname);
RD_API_EXPORT RDDatabase* RDDatabase_Create();
RD_API_EXPORT const char* RDDatabase_GetName(const RDDatabase* db);
RD_API_EXPORT const char* RDDatabase_Decompile(const RDDatabase* db);
RD_API_EXPORT bool RDDatabase_Compile(const RDDatabase* db, const char* filepath);
RD_API_EXPORT bool RDDatabase_Query(const RDDatabase* db, const char* q, RDDatabaseValue* dbvalue);
RD_API_EXPORT bool RDDatabase_Add(RDDatabase* db, const char* dbpath, const char* filepath);
RD_API_EXPORT bool RDDatabase_WriteString(RDDatabase* db, const char* path, const char* s);
RD_API_EXPORT bool RDDatabase_WriteType(RDDatabase* db, const char* path, const RDType* t);
RD_API_EXPORT void RDDatabase_SetName(RDDatabase* db, const char* name);
