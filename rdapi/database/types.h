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

typedef bool(*Callback_StructureFields)(const char* name, const RDType* type, void* userdata);

RD_API_EXPORT RDType* RDType_CreateInt(size_t size, bool issigned);
RD_API_EXPORT RDType* RDType_CreateFloat(size_t size, bool issigned);
RD_API_EXPORT RDType* RDType_CreateStructure();
RD_API_EXPORT size_t RDType_GetSize(const RDType* type);
RD_API_EXPORT rd_type RDType_GetType(const RDType* type);
RD_API_EXPORT const char* RDType_GetName(const RDType* type);

RD_API_EXPORT bool RDStructure_Append(RDType* s, RDType* t, const char* name);
RD_API_EXPORT void RDStructure_GetFields(const RDType* s, Callback_StructureFields cb, void* userdata);
