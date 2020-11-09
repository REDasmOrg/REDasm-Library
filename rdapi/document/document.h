#pragma once

#include "../types.h"
#include "../macros.h"
#include "../symbol.h"
#include "block.h"

#define RD_ENTRY_NAME "__redasm_entry__"

enum RDDocumentItemType {
    DocumentItemType_None,

    DocumentItemType_Empty,
    DocumentItemType_Segment,
    DocumentItemType_Function,
    DocumentItemType_Type,
    DocumentItemType_Symbol,
    DocumentItemType_Meta,
    DocumentItemType_Instruction,
    DocumentItemType_Unexplored,
    DocumentItemType_Separator,

    DocumentItemType_Length,
    DocumentItemType_All,
};

enum RDDocumentAction {
    DocumentAction_ItemChanged,
    DocumentAction_ItemInserted,
    DocumentAction_ItemRemoved,
};

typedef struct RDDocumentItem {
    rd_address address;
    rd_type type;
    u16 index;
} RDDocumentItem;

#define RD_DOCUMENT_ITEM(address, type, index) { address, type, index}

struct RDGraph;

DECLARE_HANDLE(RDDocument);

typedef bool (*Callback_DocumentItem)(const RDDocumentItem* item, void* userdata);
typedef bool (*Callback_Segment)(const RDSegment* segment, void* userdata);
typedef bool (*Callback_Address)(rd_address address, void* userdata);

RD_API_EXPORT bool RDDocument_GetSegmentAddress(const RDDocument* d, rd_address address, RDSegment* segment);
RD_API_EXPORT bool RDDocument_GetSegmentOffset(const RDDocument* d, rd_offset offset, RDSegment* segment);
RD_API_EXPORT bool RDDocument_GetBlock(const RDDocument* d, rd_address address, RDBlock* block);
RD_API_EXPORT bool RDDocument_GetSymbolByAddress(const RDDocument* d, rd_address address, RDSymbol* symbol);
RD_API_EXPORT bool RDDocument_GetSymbolByName(const RDDocument* d, const char* name, RDSymbol* symbol);
RD_API_EXPORT bool RDDocument_GetFunctionGraph(const RDDocument* d, rd_address address, RDGraph** item);
RD_API_EXPORT bool RDDocument_GetAny(const RDDocument* d, rd_address address, const rd_type* types, RDDocumentItem* item);
RD_API_EXPORT bool RDDocument_Rename(RDDocument* d, rd_address address, const char* newname);
RD_API_EXPORT bool RDDocument_Contains(const RDDocument* d, const RDDocumentItem* item);
RD_API_EXPORT const char* RDDocument_GetSymbolName(const RDDocument* d, rd_address address);
RD_API_EXPORT const char* RDDocument_GetComments(const RDDocument* d, rd_address address, const char* separator);
RD_API_EXPORT const RDBlockContainer* RDDocument_GetBlocks(const RDDocument* d, rd_address address);
RD_API_EXPORT RDLocation RDDocument_GetEntryPoint(const RDDocument* d);
RD_API_EXPORT RDLocation RDDocument_GetFunctionStart(const RDDocument* d, rd_address address);
RD_API_EXPORT size_t RDDocument_GetSize(const RDDocument* d);
RD_API_EXPORT bool RDDocument_AddSegmentSize(RDDocument* d, const char* name, rd_offset offset, rd_address address, u64 psize, u64 vsize, rd_flag flags);
RD_API_EXPORT bool RDDocument_AddSegmentRange(RDDocument* d, const char* name, rd_offset offset, rd_address startaddress, rd_address endaddress, rd_flag flags);
RD_API_EXPORT bool RDDocument_AddSegment(RDDocument* d, const char* name, rd_offset offset, rd_address address, u64 size, rd_flag flags);
RD_API_EXPORT bool RDDocument_SetEntry(RDDocument* d, rd_address address);
RD_API_EXPORT void RDDocument_Comment(RDDocument* d, rd_address address, const char* comment);
RD_API_EXPORT void RDDocument_AddAutoComment(RDDocument* d, rd_address address, const char* comment);
RD_API_EXPORT bool RDDocument_AddImported(RDDocument* d, rd_address address, size_t size, const char* name);
RD_API_EXPORT bool RDDocument_AddExported(RDDocument* d, rd_address address, size_t size, const char* name);
RD_API_EXPORT bool RDDocument_AddExportedFunction(RDDocument* d, rd_address address, const char* name);
RD_API_EXPORT bool RDDocument_AddAsciiString(RDDocument* d, rd_address address, size_t size, const char* name);
RD_API_EXPORT bool RDDocument_AddWideString(RDDocument* d, rd_address address, size_t size, const char* name);
RD_API_EXPORT bool RDDocument_AddPointer(RDDocument* d, rd_address address, rd_type type, const char* name);
RD_API_EXPORT bool RDDocument_AddData(RDDocument* d, rd_address address, size_t size, const char* name);
RD_API_EXPORT bool RDDocument_AddFunction(RDDocument* d, rd_address address, const char* name);
RD_API_EXPORT void RDDocument_AddSeparator(RDDocument* d, rd_address address);
RD_API_EXPORT void RDDocument_AddEmpty(RDDocument* d, rd_address address);
RD_API_EXPORT void RDDocument_Each(const RDDocument* d, Callback_DocumentItem cb, void* userdata);
RD_API_EXPORT void RDDocument_EachFunction(const RDDocument* d, Callback_Address cb, void* userdata);
RD_API_EXPORT void RDDocument_EachSegment(const RDDocument* d, Callback_Segment cb, void* userdata);

// UserData
RD_API_EXPORT bool RDDocument_SetSegmentUserData(RDDocument* d, rd_address address, uintptr_t userdata);
