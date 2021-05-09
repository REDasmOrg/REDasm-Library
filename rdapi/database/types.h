#pragma once

#include "../macros.h"
#include "../types.h"

RD_HANDLE(RDType);

enum RDTypeType {
    Type_None, Type_Void,
    Type_Bool, Type_Char,
    Type_Int, Type_Float,
    Type_AsciiString, Type_WideString,

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
RD_API_EXPORT RDType* RDType_CreateStructure(const char* name);
RD_API_EXPORT RDType* RDType_CreateArray(size_t itemscount, RDType* itemtype);
RD_API_EXPORT RDType* RDType_CreateAsciiString(size_t size);
RD_API_EXPORT RDType* RDType_CreateWideString(size_t size);
RD_API_EXPORT RDType* RDType_Clone(const RDType* type);
RD_API_EXPORT size_t RDType_GetSize(const RDType* type);
RD_API_EXPORT rd_type RDType_GetType(const RDType* type);
RD_API_EXPORT const char* RDType_GetTypeName(const RDType* type);
RD_API_EXPORT const char* RDType_GetName(const RDType* type);
RD_API_EXPORT void RDType_SetName(RDType* type, const char* n);

RD_API_EXPORT bool RDStructure_Append(RDType* s, RDType* t, const char* name);
RD_API_EXPORT void RDStructure_GetFields(const RDType* s, Callback_StructureFields cb, void* userdata);

RD_API_EXPORT size_t RDArray_GetItemsCount(const RDType* s);
RD_API_EXPORT const RDType* RDArray_GetType(const RDType* s);
