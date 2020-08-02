#pragma once

#include "../macros.h"
#include "../types.h"
#include "types.h"

enum RDDatabaseItemTypes {
    DatabaseItemType_Null = 0,

    DatabaseItemType_String,
    DatabaseItemType_Float,
    DatabaseItemType_UInt,
    DatabaseItemType_Int,
    DatabaseItemType_Bool,

    DatabaseItemType_Category,
    DatabaseItemType_Enum,
    DatabaseItemType_Union,
    DatabaseItemType_Struct,
    DatabaseItemType_Typedef,
    DatabaseItemType_Function,
};

typedef struct RDDatabaseItem {
    rd_type type;

    union {
        const char* s_value;
        double f_value;
        s64 i_value;
        u64 u_value;
        bool b_value;
    };
} RDDatabaseItem;

DECLARE_HANDLE(RDDatabase);
DECLARE_HANDLE(RDDatabaseItemNew);

RD_API_EXPORT RDDatabase* RDDatabase_Open(const char* dbname);
RD_API_EXPORT bool RDDatabase_Select(RDDatabase* db, const char* obj);
RD_API_EXPORT bool RDDatabase_Find(const RDDatabase* db, const char* key, RDDatabaseItem* item);

RD_API_EXPORT RDDatabase* RDDatabase_Create(const char* dbname);
RD_API_EXPORT RDDatabase* RDDatabase_Load(const char* dbname);
RD_API_EXPORT bool RDDatabase_Save(const RDDatabase* db, const char* filepath);
RD_API_EXPORT RDDatabaseItemNew* RDDatabase_Set(RDDatabase* db, const char* name, rd_type type);
RD_API_EXPORT RDDatabaseItemNew* RDDatabase_Get(const RDDatabase* db, const char* name);

RD_API_EXPORT RDDatabaseItemNew* RDDatabaseItem_Set(RDDatabaseItemNew* dbitem, const char* name, rd_type type);
RD_API_EXPORT RDDatabaseItemNew* RDDatabaseItem_Get(const RDDatabaseItemNew* dbitem, const char* name);
RD_API_EXPORT RDDatabaseItemNew* RDDatabaseItem_GetParent(const RDDatabaseItemNew* dbitem);
RD_API_EXPORT RDDatabase* RDDatabaseItem_GetDatabase(const RDDatabaseItemNew* dbitem);
