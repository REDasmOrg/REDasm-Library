#include "document.h"
#include <rdcore/document/backend/symboltable.h>
#include <rdcore/builtin/graph/functiongraph.h>
#include <rdcore/document/document.h>

static inline const SafeDocument& docptr(const RDDocument* d) { return *reinterpret_cast<const SafeDocument*>(d); }
static inline SafeDocument& docptr(RDDocument* d) { return *reinterpret_cast<SafeDocument*>(d); }

bool RDDocument_GetSegmentAddress(const RDDocument* d, rd_address address, RDSegment* segment) { return docptr(d)->segment(address, segment); }
bool RDDocument_GetSegmentOffset(const RDDocument* d, rd_offset offset, RDSegment* segment) { return docptr(d)->segmentOffset(offset, segment); }
bool RDDocument_GetBlock(const RDDocument* d, rd_address address, RDBlock* block) { return docptr(d)->block(address, block); }
bool RDDocument_GetSymbolByAddress(const RDDocument* d, rd_address address, RDSymbol* symbol) { return docptr(d)->symbol(address, symbol); }
bool RDDocument_GetSymbolByName(const RDDocument* d, const char* name, RDSymbol* symbol) { return docptr(d)->symbol(name, symbol); }
bool RDDocument_Rename(RDDocument* d, rd_address address, const char* newname) { return docptr(d)->rename(address, newname ? newname : std::string()); }
bool RDDocument_Contains(const RDDocument* d, const RDDocumentItem* item) { return docptr(d)->contains(item); }
bool RDDocument_GetAny(const RDDocument* d, rd_address address, const rd_type* types, RDDocumentItem* item) { return docptr(d)->getAny(address, types, item); }
const char* RDDocument_GetSymbolName(const RDDocument* d, rd_address address) { return docptr(d)->symbols()->getName(address); }
const RDBlockContainer* RDDocument_GetBlocks(const RDDocument* d, rd_address address) { return CPTR(const RDBlockContainer, docptr(d)->blocks(address)); }
size_t RDDocument_GetSize(const RDDocument* d) { return docptr(d)->size(); }
bool RDDocument_AddSegmentSize(RDDocument* d, const char* name, rd_offset offset, rd_address address, u64 psize, u64 vsize, rd_flag flags) { return docptr(d)->segment(name, offset, address, psize, vsize, flags); }
bool RDDocument_AddSegment(RDDocument* d, const char* name, rd_offset offset, rd_address address, u64 size, rd_flag flags) { return docptr(d)->segment(name, offset, address, size, size, flags); }
bool RDDocument_SetEntry(RDDocument* d, rd_address address) { return docptr(d)->entry(address); }
void RDDocument_Comment(RDDocument* d, rd_address address, const char* comment) { return docptr(d)->comment(address, comment); }
void RDDocument_AddAutoComment(RDDocument* d, rd_address address, const char* comment) { return docptr(d)->autoComment(address, comment); }
bool RDDocument_AddImported(RDDocument* d, rd_address address, size_t size, const char* name) { return docptr(d)->imported(address, size, name ? name : std::string()); }
bool RDDocument_AddExported(RDDocument* d, rd_address address, size_t size, const char* name) { return docptr(d)->exported(address, size, name ? name : std::string()); }
bool RDDocument_AddExportedFunction(RDDocument* d, rd_address address, const char* name) { return docptr(d)->exportedFunction(address, name ? name : std::string()); }
bool RDDocument_AddAsciiString(RDDocument* d, rd_address address, size_t size, const char* name) { return docptr(d)->asciiString(address, size, name ? name : std::string()); }
bool RDDocument_AddWideString(RDDocument* d, rd_address address, size_t size, const char* name) { return docptr(d)->wideString(address, size, name ? name : std::string() ); }
bool RDDocument_AddPointer(RDDocument* d, rd_address address, rd_type type, const char* name) { return docptr(d)->pointer(address, type, name ? name : std::string()); }
bool RDDocument_AddData(RDDocument* d, rd_address address, size_t size, const char* name) { return docptr(d)->data(address, size, name ? name : std::string()); }
bool RDDocument_AddFunction(RDDocument* d, rd_address address, const char* name) { return docptr(d)->function(address, name ? name : std::string() ); }
void RDDocument_AddSeparator(RDDocument* d, rd_address address) { return docptr(d)->separator(address); }
void RDDocument_AddEmpty(RDDocument* d, rd_address address) { return docptr(d)->empty(address); }

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

bool RDDocument_AddSegmentRange(RDDocument* d, const char* name, rd_offset offset, rd_address startaddress, rd_address endaddress, rd_flag flags)
{
    size_t range = endaddress - startaddress;
    return docptr(d)->segment(name, offset, startaddress, range, range, flags);
}

bool RDDocument_SetSegmentUserData(RDDocument* d, rd_address address, uintptr_t userdata) { return docptr(d)->setSegmentUserData(address, userdata); }

void RDDocument_Each(const RDDocument* d, Callback_DocumentItem cb, void* userdata)
{
    if(cb) docptr(d)->items()->each([&](const RDDocumentItem& item) {
        return cb(&item, userdata);
    });
}

void RDDocument_EachFunction(const RDDocument* d, Callback_Address cb, void* userdata)
{
    if(cb) docptr(d)->functions()->each([&](rd_address address) {
        return cb(address, userdata);
    });
}

void RDDocument_EachSegment(const RDDocument* d, Callback_Segment cb, void* userdata)
{
    if(cb) docptr(d)->segments()->each([&](const RDSegment& segment) {
        return cb(&segment, userdata);
    });
}

void RDDocument_EachSymbol(const RDDocument* d, Callback_Address cb, void* userdata)
{
    if(cb) docptr(d)->symbols()->each([&](rd_address address) {
        return cb(address, userdata);
    });
}

void RDDocument_EachSymbolByType(const RDDocument* d, rd_type type, Callback_Address cb, void* userdata)
{
    if(cb) docptr(d)->symbols()->eachType(type, [&](rd_address address) {
        return cb(address, userdata);
    });
}

bool RDDocument_FindSymbol(const RDDocument* d, const char* q, RDSymbol* symbol, rd_type hint)
{
    if(!q) return false;
    return docptr(d)->symbols()->find(q, symbol, hint);
}

bool RDDocument_FindSymbolR(const RDDocument* d, const char* q, RDSymbol* symbol, rd_type hint)
{
    if(!q) return false;
    return docptr(d)->symbols()->findR(q, symbol, hint);
}

size_t RDDocument_FindAllSymbols(const RDDocument* d, const char* q, const RDSymbol** symbols, rd_type hint)
{
    if(!q) return false;
    return docptr(d)->symbols()->findAll(q, symbols, hint);
}

size_t RDDocument_FindAllSymbolsR(const RDDocument* d, const char* q, const RDSymbol** symbols, rd_type hint)
{
    if(!q) return false;
    return docptr(d)->symbols()->findAllR(q, symbols, hint);
}

bool RDDocument_AddTypeName(RDDocument* d, rd_address address, const char* q)
{
    if(!q) return false;
    return docptr(d)->typeName(address, q);
}

bool RDDocument_AddType(RDDocument* d, rd_address address, const RDType* t)
{
    if(!t) return false;
    return docptr(d)->type(address, CPTR(const Type, t));
}
