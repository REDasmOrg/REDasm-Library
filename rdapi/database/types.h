#pragma once

#include "../macros.h"
#include "../types.h"

RD_HANDLE(RDType);

enum RDTypes {
    Type_None,
    Type_Void,
    Type_Bool,
    Type_Char,
    Type_Int,
    Type_Float,

    Type_Array,

    Type_Function,
    Type_Alias,
    Type_Structure,
    Type_Union,
    Type_Enum,
};

RD_API_EXPORT RDType* RDType_CreateInt(size_t size, bool issigned);
RD_API_EXPORT RDType* RDType_CreateFloat(size_t size, bool issigned);
RD_API_EXPORT RDType* RDType_CreateStructure();
RD_API_EXPORT rd_type RDType_GetType(const RDType* type);

RD_API_EXPORT bool RDStructure_Append(RDType* s, RDType* t, const char* name);
