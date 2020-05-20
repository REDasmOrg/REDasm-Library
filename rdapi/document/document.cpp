#include "document.h"
#include <rdcore/document/backend/symboltable.h>
#include <rdcore/document/document.h>
#include <rdcore/builtin/graph/functiongraph.h>

static inline const SafeDocument& docptr(const RDDocument* d) { return *reinterpret_cast<const SafeDocument*>(d); }
static inline SafeDocument& docptr(RDDocument* d) { return *reinterpret_cast<SafeDocument*>(d); }

bool RDDocument_GetSegmentAddress(const RDDocument* d, address_t address, RDSegment* segment) { return docptr(d)->segment(address, segment); }
bool RDDocument_GetSegmentOffset(const RDDocument* d, offset_t offset, RDSegment* segment) { return docptr(d)->segmentOffset(offset, segment); }
bool RDDocument_GetSegmentAt(const RDDocument* d, size_t index, RDSegment* segment) { return docptr(d)->segmentAt(index, segment); }
bool RDDocument_GetBlock(const RDDocument* d, address_t address, RDBlock* block) { return docptr(d)->block(address, block); }
bool RDDocument_GetBlockAt(const RDDocument* d, size_t index, RDBlock* block) { return docptr(d)->blockAt(index, block); }
bool RDDocument_GetItemAt(const RDDocument* d, size_t index, RDDocumentItem* item) { return docptr(d)->itemAt(index, item); }
bool RDDocument_GetFunctionItem(const RDDocument* d, address_t address, RDDocumentItem* item) { return docptr(d)->functionItem(address, item); }
bool RDDocument_GetInstructionItem(const RDDocument* d, address_t address, RDDocumentItem* item) { return docptr(d)->instructionItem(address, item); }
bool RDDocument_GetSymbolItem(const RDDocument* d, address_t address, RDDocumentItem* item) { return docptr(d)->symbolItem(address, item); }
bool RDDocument_GetSymbolByAddress(const RDDocument* d, address_t address, RDSymbol* symbol) { return docptr(d)->symbol(address, symbol); }
bool RDDocument_GetSymbolByName(const RDDocument* d, const char* name, RDSymbol* symbol) { return docptr(d)->symbol(name, symbol); }
bool RDDocument_PrevInstruction(const RDDocument* d, const RDInstruction* instruction, RDInstruction** previnstruction) { return docptr(d)->prevInstruction(instruction, previnstruction); }
bool RDDocument_LockInstruction(const RDDocument* d, address_t address, RDInstruction** instruction) { return docptr(d)->lockInstruction(address, instruction); }
bool RDDocument_UnlockInstruction(const RDDocument* d, RDInstruction* instruction) { return docptr(d)->unlockInstruction(instruction);  }
const char* RDDocument_GetSymbolName(const RDDocument* d, address_t address) { return docptr(d)->symbols()->getName(address); }
RDLocation RDDocument_GetFunctionAt(const RDDocument* d, size_t index) { return docptr(d)->functionAt(index); }
RDLocation RDDocument_EntryPoint(const RDDocument* d) { return docptr(d)->entryPoint(); }
RDLocation RDDocument_FunctionStart(const RDDocument* d, address_t address) { return docptr(d)->functionStart(address); }
size_t RDDocument_ItemIndex(const RDDocument* d, const RDDocumentItem* item) { return docptr(d)->itemIndex(item); }
size_t RDDocument_FunctionIndex(const RDDocument* d, address_t address) { return docptr(d)->functionIndex(address); }
size_t RDDocument_InstructionIndex(const RDDocument* d, address_t address) { return docptr(d)->instructionIndex(address); }
size_t RDDocument_SymbolIndex(const RDDocument* d, address_t address) { return docptr(d)->symbolIndex(address); }
size_t RDDocument_GetItemList(const RDDocument* d, size_t startindex, size_t count, RDDocumentItem* items) { return docptr(d)->itemsAt(startindex, count, items); }
size_t RDDocument_ItemsCount(const RDDocument* d) { return docptr(d)->itemsCount(); }
size_t RDDocument_SegmentsCount(const RDDocument* d) { return docptr(d)->segmentsCount(); }
size_t RDDocument_BlockCount(const RDDocument* d) { return docptr(d)->blocksCount(); }
size_t RDDocument_FunctionsCount(const RDDocument* d) { return docptr(d)->functionsCount(); }
size_t RDDocument_SymbolsCount(const RDDocument* d) { return docptr(d)->symbolsCount(); }
void RDDocument_SetEntry(RDDocument* d, address_t address) { docptr(d)->entry(address); }
void RDDocument_AddSegmentSize(RDDocument* d, const char* name, offset_t offset, address_t address, u64 psize, u64 vsize, type_t type) { docptr(d)->segment(name, offset, address, psize, vsize, type); }
void RDDocument_AddSegment(RDDocument* d, const char* name, offset_t offset, address_t address, u64 size, type_t type) { docptr(d)->segment(name, offset, address, size, size, type); }
void RDDocument_AddImported(RDDocument* d, address_t address, size_t size, const char* name) { docptr(d)->imported(address, size, name); }
void RDDocument_AddExported(RDDocument* d, address_t address, const char* name) { docptr(d)->exported(address, name); }
void RDDocument_AddExportedFunction(RDDocument* d, address_t address, const char* name) { docptr(d)->exportedFunction(address, name); }
void RDDocument_AddInstruction(RDDocument* d, const RDInstruction* instruction) { docptr(d)->instruction(instruction); }
void RDDocument_AddAsciiString(RDDocument* d, address_t address, size_t size) { docptr(d)->asciiString(address, size); }
void RDDocument_AddWideString(RDDocument* d, address_t address, size_t size) { docptr(d)->wideString(address, size); }
void RDDocument_AddPointer(RDDocument* d, address_t address, type_t type, const char* name) { docptr(d)->pointer(address, type, name); }
void RDDocument_AddData(RDDocument* d, address_t address, size_t size, const char* name) { docptr(d)->data(address, size, name); }
void RDDocument_AddFunction(RDDocument* d, address_t address, const char* name) { docptr(d)->function(address, name); }
void RDDocument_AddSeparator(RDDocument* d, address_t address) { docptr(d)->separator(address); }
void RDDocument_AddEmpty(RDDocument* d, address_t address) { docptr(d)->empty(address); }

bool RDDocument_GetFunctionGraph(const RDDocument* d, address_t address, RDGraph** item)
{
     FunctionGraph* g = docptr(d)->graph(address);
     if(item && g) *item = CPTR(RDGraph, g);
     return g;
}
