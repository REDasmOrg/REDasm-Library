#pragma once

#include "macros.h"
#include "types.h"

enum RDDatabaseItemType {
    DatabaseItemType_None,
    DatabaseItemType_String,
    DatabaseItemType_Float,
    DatabaseItemType_UInt,
    DatabaseItemType_Int,
    DatabaseItemType_Bool,
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

RD_API_EXPORT RDDatabase* RDDatabase_Open(const char* dbname);
RD_API_EXPORT bool RDDatabase_Select(RDDatabase* db, const char* obj);
RD_API_EXPORT bool RDDatabase_Find(const RDDatabase* db, const char* key, RDDatabaseItem* item);
