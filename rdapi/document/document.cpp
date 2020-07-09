#include "document.h"
#include <rdcore/document/backend/symboltable.h>
#include <rdcore/builtin/graph/functiongraph.h>
#include <rdcore/document/document.h>

static inline const SafeDocument& docptr(const RDDocument* d) { return *reinterpret_cast<const SafeDocument*>(d); }
static inline SafeDocument& docptr(RDDocument* d) { return *reinterpret_cast<SafeDocument*>(d); }

bool RDDocument_GetSegmentAddress(const RDDocument* d, rd_address address, RDSegment* segment) { return docptr(d)->segment(address, segment); }
bool RDDocument_GetSegmentOffset(const RDDocument* d, rd_offset offset, RDSegment* segment) { return docptr(d)->segmentOffset(offset, segment); }
bool RDDocument_GetSegmentAt(const RDDocument* d, size_t index, RDSegment* segment) { return docptr(d)->segmentAt(index, segment); }
bool RDDocument_GetBlock(const RDDocument* d, rd_address address, RDBlock* block) { return docptr(d)->block(address, block); }
bool RDDocument_GetBlockAt(const RDDocument* d, size_t index, RDBlock* block) { return docptr(d)->blockAt(index, block); }
bool RDDocument_GetItemAt(const RDDocument* d, size_t index, RDDocumentItem* item) { return docptr(d)->itemAt(index, item); }
bool RDDocument_GetFunctionItem(const RDDocument* d, rd_address address, RDDocumentItem* item) { return docptr(d)->functionItem(address, item); }
bool RDDocument_GetInstructionItem(const RDDocument* d, rd_address address, RDDocumentItem* item) { return docptr(d)->instructionItem(address, item); }
bool RDDocument_GetSymbolItem(const RDDocument* d, rd_address address, RDDocumentItem* item) { return docptr(d)->symbolItem(address, item); }
bool RDDocument_GetSymbolByAddress(const RDDocument* d, rd_address address, RDSymbol* symbol) { return docptr(d)->symbol(address, symbol); }
bool RDDocument_GetSymbolByName(const RDDocument* d, const char* name, RDSymbol* symbol) { return docptr(d)->symbol(name, symbol); }
bool RDDocument_PrevInstruction(const RDDocument* d, const RDInstruction* instruction, RDInstruction** previnstruction) { return docptr(d)->prevInstruction(instruction, previnstruction); }
bool RDDocument_LockInstruction(const RDDocument* d, rd_address address, RDInstruction** instruction) { return docptr(d)->lockInstruction(address, instruction); }
bool RDDocument_UnlockInstruction(const RDDocument* d, RDInstruction* instruction) { return docptr(d)->unlockInstruction(instruction);  }
bool RDDocument_Rename(RDDocument* d, rd_address address, const char* newname) { return docptr(d)->rename(address, newname ? newname : std::string()); }
const char* RDDocument_GetSymbolName(const RDDocument* d, rd_address address) { return docptr(d)->symbols()->getName(address); }
RDLocation RDDocument_GetFunctionAt(const RDDocument* d, size_t index) { return docptr(d)->functionAt(index); }
RDLocation RDDocument_EntryPoint(const RDDocument* d) { return docptr(d)->entryPoint(); }
RDLocation RDDocument_FunctionStart(const RDDocument* d, rd_address address) { return docptr(d)->functionStart(address); }
size_t RDDocument_ItemIndex(const RDDocument* d, const RDDocumentItem* item) { return docptr(d)->itemIndex(item); }
size_t RDDocument_FunctionIndex(const RDDocument* d, rd_address address) { return docptr(d)->functionIndex(address); }
size_t RDDocument_InstructionIndex(const RDDocument* d, rd_address address) { return docptr(d)->instructionIndex(address); }
size_t RDDocument_SymbolIndex(const RDDocument* d, rd_address address) { return docptr(d)->symbolIndex(address); }
size_t RDDocument_GetItemList(const RDDocument* d, size_t startindex, size_t count, RDDocumentItem* items) { return docptr(d)->itemsAt(startindex, count, items); }
size_t RDDocument_ItemsCount(const RDDocument* d) { return docptr(d)->itemsCount(); }
size_t RDDocument_SegmentsCount(const RDDocument* d) { return docptr(d)->segmentsCount(); }
size_t RDDocument_BlockCount(const RDDocument* d) { return docptr(d)->blocksCount(); }
size_t RDDocument_FunctionsCount(const RDDocument* d) { return docptr(d)->functionsCount(); }
size_t RDDocument_SymbolsCount(const RDDocument* d) { return docptr(d)->symbolsCount(); }
void RDDocument_SetEntry(RDDocument* d, rd_address address) { docptr(d)->entry(address); }
void RDDocument_Comment(RDDocument* d, rd_address address, const char* comment) { docptr(d)->comment(address, comment); }
void RDDocument_AddAutoComment(RDDocument* d, rd_address address, const char* comment) { docptr(d)->autoComment(address, comment); }
void RDDocument_AddSegmentSize(RDDocument* d, const char* name, rd_offset offset, rd_address address, u64 psize, u64 vsize, rd_flag flags) { docptr(d)->segment(name, offset, address, psize, vsize, flags); }
void RDDocument_AddSegment(RDDocument* d, const char* name, rd_offset offset, rd_address address, u64 size, rd_flag flags) { docptr(d)->segment(name, offset, address, size, size, flags); }
void RDDocument_AddImported(RDDocument* d, rd_address address, size_t size, const char* name) { docptr(d)->imported(address, size, name ? name : std::string()); }
void RDDocument_AddExported(RDDocument* d, rd_address address, size_t size, const char* name) { docptr(d)->exported(address, size, name ? name : std::string()); }
void RDDocument_AddExportedFunction(RDDocument* d, rd_address address, const char* name) { docptr(d)->exportedFunction(address, name ? name : std::string()); }
void RDDocument_AddInstruction(RDDocument* d, const RDInstruction* instruction) { docptr(d)->instruction(instruction); }
void RDDocument_AddAsciiString(RDDocument* d, rd_address address, size_t size, const char* name) { docptr(d)->asciiString(address, size, name ? name : std::string()); }
void RDDocument_AddWideString(RDDocument* d, rd_address address, size_t size, const char* name) { docptr(d)->wideString(address, size, name ? name : std::string() ); }
void RDDocument_AddPointer(RDDocument* d, rd_address address, rd_type type, const char* name) { docptr(d)->pointer(address, type, name ? name : std::string()); }
void RDDocument_AddData(RDDocument* d, rd_address address, size_t size, const char* name) { docptr(d)->data(address, size, name ? name : std::string()); }
void RDDocument_AddFunction(RDDocument* d, rd_address address, const char* name) { docptr(d)->function(address, name ? name : std::string() ); }
void RDDocument_AddSeparator(RDDocument* d, rd_address address) { docptr(d)->separator(address); }
void RDDocument_AddEmpty(RDDocument* d, rd_address address) { docptr(d)->empty(address); }

bool RDDocument_GetFunctionGraph(const RDDocument* d, rd_address address, RDGraph** item)
{
     FunctionGraph* g = docptr(d)->graph(address);
     if(item && g) *item = CPTR(RDGraph, g);
     return g;
}

const char* RDDocument_GetComments(const RDDocument* d, rd_address address, const char* separator)
{
    static std::string s;
    s = docptr(d)->comment(address, true, separator);
    return s.c_str();
}

void RDDocument_AddSegmentRange(RDDocument* d, const char* name, rd_offset offset, rd_address startaddress, rd_address endaddress, rd_flag flags)
{
    size_t range = endaddress - startaddress;
    docptr(d)->segment(name, offset, startaddress, range, range, flags);
}
