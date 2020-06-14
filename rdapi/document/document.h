#pragma once

#include "../types.h"
#include "../macros.h"
#include "../symbol.h"
#include "block.h"

enum RDDocumentItemType {
    DocumentItemType_None,

    DocumentItemType_First,
    DocumentItemType_Segment = DocumentItemType_First,
    DocumentItemType_Empty,
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

struct RDInstruction;
struct RDGraph;

DECLARE_HANDLE(RDDocument);

RD_API_EXPORT bool RDDocument_GetSegmentAddress(const RDDocument* d, rd_address address, RDSegment* segment);
RD_API_EXPORT bool RDDocument_GetSegmentOffset(const RDDocument* d, rd_offset offset, RDSegment* segment);
RD_API_EXPORT bool RDDocument_GetSegmentAt(const RDDocument* d, size_t index, RDSegment* segment);
RD_API_EXPORT bool RDDocument_GetBlock(const RDDocument* d, rd_address address, RDBlock* block);
RD_API_EXPORT bool RDDocument_GetBlockAt(const RDDocument* d, size_t index, RDBlock* block);
RD_API_EXPORT bool RDDocument_GetItemAt(const RDDocument* d, size_t index, RDDocumentItem* item);
RD_API_EXPORT bool RDDocument_GetFunctionItem(const RDDocument* d, rd_address address, RDDocumentItem* item);
RD_API_EXPORT bool RDDocument_GetInstructionItem(const RDDocument* d, rd_address address, RDDocumentItem* item);
RD_API_EXPORT bool RDDocument_GetSymbolItem(const RDDocument* d, rd_address address, RDDocumentItem* item);
RD_API_EXPORT bool RDDocument_GetSymbolByAddress(const RDDocument* d, rd_address address, RDSymbol* symbol);
RD_API_EXPORT bool RDDocument_GetSymbolByName(const RDDocument* d, const char* name, RDSymbol* symbol);
RD_API_EXPORT bool RDDocument_GetFunctionGraph(const RDDocument* d, rd_address address, RDGraph** item);
RD_API_EXPORT bool RDDocument_PrevInstruction(const RDDocument* d, const RDInstruction* instruction, RDInstruction** previnstruction);
RD_API_EXPORT bool RDDocument_LockInstruction(const RDDocument* d, rd_address address, RDInstruction** instruction);
RD_API_EXPORT bool RDDocument_UnlockInstruction(const RDDocument* d, RDInstruction* instruction);
RD_API_EXPORT bool RDDocument_Rename(RDDocument* d, rd_address address, const char* newname);
RD_API_EXPORT const char* RDDocument_GetSymbolName(const RDDocument* d, rd_address address);
RD_API_EXPORT const char* RDDocument_GetComments(const RDDocument* d, rd_address address, const char* separator);
RD_API_EXPORT RDLocation RDDocument_GetFunctionAt(const RDDocument* d, size_t index);
RD_API_EXPORT RDLocation RDDocument_EntryPoint(const RDDocument* d);
RD_API_EXPORT RDLocation RDDocument_FunctionStart(const RDDocument* d, rd_address address);
RD_API_EXPORT size_t RDDocument_ItemIndex(const RDDocument* d, const RDDocumentItem* item);
RD_API_EXPORT size_t RDDocument_FunctionIndex(const RDDocument* d, rd_address address);
RD_API_EXPORT size_t RDDocument_InstructionIndex(const RDDocument* d, rd_address address);
RD_API_EXPORT size_t RDDocument_SymbolIndex(const RDDocument* d, rd_address address);
RD_API_EXPORT size_t RDDocument_GetItemList(const RDDocument* d, size_t startindex, size_t count, RDDocumentItem* items);
RD_API_EXPORT size_t RDDocument_BlockCount(const RDDocument* d);
RD_API_EXPORT size_t RDDocument_ItemsCount(const RDDocument* d);
RD_API_EXPORT size_t RDDocument_SegmentsCount(const RDDocument* d);
RD_API_EXPORT size_t RDDocument_FunctionsCount(const RDDocument* d);
RD_API_EXPORT size_t RDDocument_SymbolsCount(const RDDocument* d);
RD_API_EXPORT void RDDocument_SetEntry(RDDocument* d, rd_address address);
RD_API_EXPORT void RDDocument_Comment(RDDocument* d, rd_address address, const char* comment);
RD_API_EXPORT void RDDocument_AddAutoComment(RDDocument* d, rd_address address, const char* comment);
RD_API_EXPORT void RDDocument_AddSegmentSize(RDDocument* d, const char* name, rd_offset offset, rd_address address, u64 psize, u64 vsize, rd_flag flags);
RD_API_EXPORT void RDDocument_AddSegment(RDDocument* d, const char* name, rd_offset offset, rd_address address, u64 size, rd_flag flags);
RD_API_EXPORT void RDDocument_AddImported(RDDocument* d, rd_address address, size_t size, const char* name);
RD_API_EXPORT void RDDocument_AddExported(RDDocument* d, rd_address address, const char* name);
RD_API_EXPORT void RDDocument_AddExportedFunction(RDDocument* d, rd_address address, const char* name);
RD_API_EXPORT void RDDocument_AddInstruction(RDDocument* d, const RDInstruction* instruction);
RD_API_EXPORT void RDDocument_AddAsciiString(RDDocument* d, rd_address address, size_t size);
RD_API_EXPORT void RDDocument_AddWideString(RDDocument* d, rd_address address, size_t size);
RD_API_EXPORT void RDDocument_AddPointer(RDDocument* d, rd_address address, rd_type type, const char* name);
RD_API_EXPORT void RDDocument_AddData(RDDocument* d, rd_address address, size_t size, const char* name);
RD_API_EXPORT void RDDocument_AddFunction(RDDocument* d, rd_address address, const char* name);
RD_API_EXPORT void RDDocument_AddSeparator(RDDocument* d, rd_address address);
RD_API_EXPORT void RDDocument_AddEmpty(RDDocument* d, rd_address address);
