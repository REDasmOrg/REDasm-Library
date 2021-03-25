#pragma once

#include "../database/types.h"
#include "../types.h"
#include "../macros.h"
#include "../types.h"
#include "block.h"

#define RD_ENTRY_NAME "__redasm_entry__"

struct RDBufferView;
struct RDGraph;

RD_HANDLE(RDDocument);

typedef bool (*Callback_Block)(const RDBlock* b, void* userdata);

RD_API_EXPORT void RDDocument_SetSegmentSize(RDDocument* d, const char* name, rd_offset offset, rd_address address, u64 psize, u64 vsize, rd_flag flags);
RD_API_EXPORT void RDDocument_SetSegmentRange(RDDocument* d, const char* name, rd_offset offset, rd_address startaddress, rd_address endaddress, rd_flag flags);
RD_API_EXPORT void RDDocument_SetSegment(RDDocument* d, const char* name, rd_offset offset, rd_address address, u64 size, rd_flag flags);
RD_API_EXPORT void RDDocument_SetFunction(RDDocument* d, rd_address address, const char* label);
RD_API_EXPORT void RDDocument_SetString(RDDocument* d, rd_address address, size_t size, rd_flag flags);
RD_API_EXPORT void RDDocument_SetData(RDDocument* d, rd_address address, size_t size, const char* label);
RD_API_EXPORT void RDDocument_SetLabel(RDDocument* d, rd_address address, rd_type type, const char* label);
RD_API_EXPORT void RDDocument_SetPointer(RDDocument* d, rd_address address, const char* label);
RD_API_EXPORT void RDDocument_SetExportedFunction(RDDocument* d, rd_address address, const char* label);
RD_API_EXPORT void RDDocument_SetExported(RDDocument* d, rd_address address, size_t size, const char* label);
RD_API_EXPORT void RDDocument_SetImported(RDDocument* d, rd_address address, size_t size, const char* label);
RD_API_EXPORT void RDDocument_SetEntry(RDDocument* d, rd_address address);
RD_API_EXPORT void RDDocument_SetTypeName(RDDocument* d, rd_address address, const char* q);
RD_API_EXPORT void RDDocument_SetType(RDDocument* d, rd_address address, const RDType* t);
RD_API_EXPORT void RDDocument_SetComments(RDDocument* d, rd_address address, const char* comments);
RD_API_EXPORT void RDDocument_AddComments(RDDocument* d, rd_address address, const char* comment);
RD_API_EXPORT void RDDocument_EachBlock(const RDDocument* d, rd_address address, Callback_Block cb, void* userdata);
RD_API_EXPORT bool RDDocument_CreateFunction(RDDocument* d, rd_address address, const char* name);
RD_API_EXPORT bool RDDocument_UpdateLabel(RDDocument* d, rd_address address, const char* label);
RD_API_EXPORT bool RDDocument_PointerToSegment(const RDDocument* d, const void* ptr, RDSegment* segment);
RD_API_EXPORT bool RDDocument_AddressToSegment(const RDDocument* d, rd_address address, RDSegment* segment);
RD_API_EXPORT bool RDDocument_OffsetToSegment(const RDDocument* d, rd_offset offset, RDSegment* segment);
RD_API_EXPORT bool RDDocument_AddressToBlock(const RDDocument* d, rd_address address, RDBlock* block);
RD_API_EXPORT bool RDDocument_GetFunctionGraph(const RDDocument* d, rd_address address, RDGraph** item);
RD_API_EXPORT bool RDDocument_FindLabel(const RDDocument* d, const char* q, rd_address* resaddress);
RD_API_EXPORT bool RDDocument_FindLabelR(const RDDocument* d, const char* q, rd_address* resaddress);
RD_API_EXPORT bool RDDocument_GetView(const RDDocument* d, rd_address address, size_t size, RDBufferView* view);
RD_API_EXPORT bool RDDocument_GetBlockView(const RDDocument* d, rd_address address, RDBufferView* view);
RD_API_EXPORT const char* RDDocument_GetLabel(const RDDocument* d, rd_address address);
RD_API_EXPORT const char* RDDocument_GetComments(const RDDocument* d, rd_address address);
RD_API_EXPORT const RDType* RDDocument_GetType(const RDDocument* d, rd_address address);
RD_API_EXPORT rd_address RDDocument_GetAddress(const RDDocument* d, const char* label);
RD_API_EXPORT rd_flag RDDocument_GetFlags(const RDDocument* d, rd_address address);
RD_API_EXPORT size_t RDDocument_FindLabels(const RDDocument* d, const char* q, const rd_address** addresses);
RD_API_EXPORT size_t RDDocument_FindLabelsR(const RDDocument* d, const char* q, const rd_address** addresses);
RD_API_EXPORT size_t RDDocument_GetLabels(const RDDocument* d, const rd_address** addresses);
RD_API_EXPORT size_t RDDocument_GetLabelsByFlag(const RDDocument* d, rd_flag flag, const rd_address** addresses);
RD_API_EXPORT size_t RDDocument_GetSegments(const RDDocument* d, const rd_address** addresses);
RD_API_EXPORT size_t RDDocument_GetFunctions(const RDDocument* d, const rd_address** addresses);
RD_API_EXPORT RDLocation RDDocument_GetFunctionStart(const RDDocument* d, rd_address address);
RD_API_EXPORT RDLocation RDDocument_GetEntry(const RDDocument* d);
