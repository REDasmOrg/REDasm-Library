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
    DocumentItemType_All
};

enum RDDocumentAction {
    DocumentAction_ItemChanged,
    DocumentAction_ItemInserted,
    DocumentAction_ItemRemoved
};

typedef struct RDDocumentItem {
    address_t address;
    type_t type;
    u16 index;
} RDDocumentItem;

struct RDInstruction;
struct RDGraph;

DECLARE_HANDLE(RDDocument);

RD_API_EXPORT bool RDDocument_GetSegmentAddress(const RDDocument* d, address_t address, RDSegment* segment);
RD_API_EXPORT bool RDDocument_GetSegmentOffset(const RDDocument* d, offset_t offset, RDSegment* segment);
RD_API_EXPORT bool RDDocument_GetSegmentAt(const RDDocument* d, size_t index, RDSegment* segment);
RD_API_EXPORT bool RDDocument_GetBlock(const RDDocument* d, address_t address, RDBlock* block);
RD_API_EXPORT bool RDDocument_GetBlockAt(const RDDocument* d, size_t index, RDBlock* block);
RD_API_EXPORT bool RDDocument_GetItemAt(const RDDocument* d, size_t index, RDDocumentItem* item);
RD_API_EXPORT bool RDDocument_GetFunctionItem(const RDDocument* d, address_t address, RDDocumentItem* item);
RD_API_EXPORT bool RDDocument_GetInstructionItem(const RDDocument* d, address_t address, RDDocumentItem* item);
RD_API_EXPORT bool RDDocument_GetSymbolItem(const RDDocument* d, address_t address, RDDocumentItem* item);
RD_API_EXPORT bool RDDocument_GetSymbolByAddress(const RDDocument* d, address_t address, RDSymbol* symbol);
RD_API_EXPORT bool RDDocument_GetSymbolByName(const RDDocument* d, const char* name, RDSymbol* symbol);
RD_API_EXPORT bool RDDocument_GetFunctionGraph(const RDDocument* d, address_t address, RDGraph** item);
RD_API_EXPORT bool RDDocument_PrevInstruction(const RDDocument* d, const RDInstruction* instruction, RDInstruction** previnstruction);
RD_API_EXPORT bool RDDocument_LockInstruction(const RDDocument* d, address_t address, RDInstruction** instruction);
RD_API_EXPORT bool RDDocument_UnlockInstruction(const RDDocument* d, RDInstruction* instruction);
RD_API_EXPORT bool RDDocument_Rename(RDDocument* d, address_t address, const char* newname);
RD_API_EXPORT const char* RDDocument_GetSymbolName(const RDDocument* d, address_t address);
RD_API_EXPORT const char* RDDocument_GetComments(const RDDocument* d, address_t address, const char* separator);
RD_API_EXPORT RDLocation RDDocument_GetFunctionAt(const RDDocument* d, size_t index);
RD_API_EXPORT RDLocation RDDocument_EntryPoint(const RDDocument* d);
RD_API_EXPORT RDLocation RDDocument_FunctionStart(const RDDocument* d, address_t address);
RD_API_EXPORT size_t RDDocument_ItemIndex(const RDDocument* d, const RDDocumentItem* item);
RD_API_EXPORT size_t RDDocument_FunctionIndex(const RDDocument* d, address_t address);
RD_API_EXPORT size_t RDDocument_InstructionIndex(const RDDocument* d, address_t address);
RD_API_EXPORT size_t RDDocument_SymbolIndex(const RDDocument* d, address_t address);
RD_API_EXPORT size_t RDDocument_GetItemList(const RDDocument* d, size_t startindex, size_t count, RDDocumentItem* items);
RD_API_EXPORT size_t RDDocument_BlockCount(const RDDocument* d);
RD_API_EXPORT size_t RDDocument_ItemsCount(const RDDocument* d);
RD_API_EXPORT size_t RDDocument_SegmentsCount(const RDDocument* d);
RD_API_EXPORT size_t RDDocument_FunctionsCount(const RDDocument* d);
RD_API_EXPORT size_t RDDocument_SymbolsCount(const RDDocument* d);
RD_API_EXPORT void RDDocument_SetEntry(RDDocument* d, address_t address);
RD_API_EXPORT void RDDocument_Comment(RDDocument* d, address_t address, const char* comment);
RD_API_EXPORT void RDDocument_AddAutoComment(RDDocument* d, address_t address, const char* comment);
RD_API_EXPORT void RDDocument_AddSegmentSize(RDDocument* d, const char* name, offset_t offset, address_t address, u64 psize, u64 vsize, type_t type);
RD_API_EXPORT void RDDocument_AddSegment(RDDocument* d, const char* name, offset_t offset, address_t address, u64 size, type_t type);
RD_API_EXPORT void RDDocument_AddImported(RDDocument* d, address_t address, size_t size, const char* name);
RD_API_EXPORT void RDDocument_AddExported(RDDocument* d, address_t address, const char* name);
RD_API_EXPORT void RDDocument_AddExportedFunction(RDDocument* d, address_t address, const char* name);
RD_API_EXPORT void RDDocument_AddInstruction(RDDocument* d, const RDInstruction* instruction);
RD_API_EXPORT void RDDocument_AddAsciiString(RDDocument* d, address_t address, size_t size);
RD_API_EXPORT void RDDocument_AddWideString(RDDocument* d, address_t address, size_t size);
RD_API_EXPORT void RDDocument_AddPointer(RDDocument* d, address_t address, type_t type, const char* name);
RD_API_EXPORT void RDDocument_AddData(RDDocument* d, address_t address, size_t size, const char* name);
RD_API_EXPORT void RDDocument_AddFunction(RDDocument* d, address_t address, const char* name);
RD_API_EXPORT void RDDocument_AddSeparator(RDDocument* d, address_t address);
RD_API_EXPORT void RDDocument_AddEmpty(RDDocument* d, address_t address);
