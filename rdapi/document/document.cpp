#include "document.h"
#include <rdcore/builtin/graph/functiongraph/functiongraph.h>
#include <rdcore/document/document.h>

static inline const SafeDocument& docptr(const RDDocument* d) { return *reinterpret_cast<const SafeDocument*>(d); }
static inline SafeDocument& docptr(RDDocument* d) { return *reinterpret_cast<SafeDocument*>(d); }

bool RDDocument_UpdateLabel(RDDocument* d, rd_address address, const char* label) { return label ? docptr(d)->updateLabel(address, label) : false; }
void RDDocument_SetData(RDDocument* d, rd_address address, size_t size, const char* label) { docptr(d)->setData(address, size, label ? label : std::string()); }
RDLocation RDDocument_GetFunctionStart(const RDDocument* d, rd_address address) { return docptr(d)->getFunctionStart(address); }
RDLocation RDDocument_GetEntry(const RDDocument* d) { return docptr(d)->getEntry(); }
RDLocation RDDocument_Dereference(const RDDocument* d, rd_address address) { return docptr(d)->dereference(address); }
bool RDDocument_FindLabel(const RDDocument* d, const char* q, rd_address* resaddress) { return q ? docptr(d)->findLabel(q, resaddress) : false; }
bool RDDocument_FindLabelR(const RDDocument* d, const char* q, rd_address* resaddress) { return q ? docptr(d)->findLabelR(q, resaddress) : false; }
void RDDocument_SetTypeName(RDDocument* d, rd_address address, const char* q) { if(q) docptr(d)->setTypeName(address, q); }
void RDDocument_SetType(RDDocument* d, rd_address address, const RDType* t) { if(t) docptr(d)->setType(address, CPTR(const Type, t)); }
void RDDocument_SetFunction(RDDocument* d, rd_address address, const char* label) { docptr(d)->setFunction(address, label ? label : std::string()); }
void RDDocument_SetString(RDDocument* d, rd_address address, size_t size, rd_flag flags) { docptr(d)->setString(address, size, flags); }
void RDDocument_SetLabel(RDDocument* d, rd_address address, rd_type type, const char* label) { docptr(d)->setLabel(address, type, label ? label : std::string()); }
void RDDocument_SetExportedFunction(RDDocument* d, rd_address address, const char* label) { docptr(d)->setExportedFunction(address, label ? label : std::string()); }
void RDDocument_SetExported(RDDocument* d, rd_address address, size_t size, const char* label) { docptr(d)->setExported(address, size, label ? label : std::string()); }
void RDDocument_SetImported(RDDocument* d, rd_address address, size_t size, const char* label) { docptr(d)->setImported(address, size, label ? label : std::string()); }
void RDDocument_SetPointer(RDDocument* d, rd_address address, const char* label) { docptr(d)->setPointer(address, label ? label : std::string()); }
void RDDocument_SetEntry(RDDocument* d, rd_address address) { docptr(d)->setEntry(address); }
void RDDocument_SetComments(RDDocument* d, rd_address address, const char* comments) { if(comments) docptr(d)->setComments(address, comments); }
void RDDocument_AddComments(RDDocument* d, rd_address address, const char* comment) { if(comment) docptr(d)->addComment(address, comment); }
bool RDDocument_CreateFunction(RDDocument* d, rd_address address, const char* name) { return docptr(d)->createFunction(address, name ? name : std::string()); }
bool RDDocument_PointerToSegment(const RDDocument* d, const void* ptr, RDSegment* segment) { return docptr(d)->pointerToSegment(ptr, segment); }
bool RDDocument_AddressToSegment(const RDDocument* d, rd_address address, RDSegment* segment) { return docptr(d)->addressToSegment(address, segment); }
bool RDDocument_OffsetToSegment(const RDDocument* d, rd_offset offset, RDSegment* segment) { return docptr(d)->offsetToSegment(offset, segment); }
bool RDDocument_AddressToBlock(const RDDocument* d, rd_address address, RDBlock* block) { return docptr(d)->addressToBlock(address, block); }
bool RDDocument_GetView(const RDDocument* d, rd_address address, size_t size, RDBufferView* view) { return docptr(d)->getView(address, size, view); }
bool RDDocument_GetBlockView(const RDDocument* d, rd_address address, RDBufferView* view) { return docptr(d)->getBlockView(address, view); }
const RDType* RDDocument_GetType(const RDDocument* d, rd_address address) { return CPTR(const RDType, docptr(d)->getType(address)); }
rd_address RDDocument_GetAddress(const RDDocument* d, const char* label) { return label ? docptr(d)->getAddress(label) : RD_NVAL; }
rd_flag RDDocument_GetFlags(const RDDocument* d, rd_address address) { return docptr(d)->getFlags(address); }
size_t RDDocument_GetLabels(const RDDocument* d, const rd_address** addresses) { return docptr(d)->getLabels(addresses); }
size_t RDDocument_GetLabelsByFlag(const RDDocument* d, rd_flag flag, const rd_address** addresses) { return docptr(d)->getLabelsByFlag(flag, addresses); }
size_t RDDocument_GetSegments(const RDDocument* d, const rd_address** addresses) { return docptr(d)->getSegments(addresses);  }
size_t RDDocument_GetFunctions(const RDDocument* d, const rd_address** addresses) { return docptr(d)->getFunctions(addresses); }
size_t RDDocument_FindLabels(const RDDocument* d, const char* q, const rd_address** resaddresses) { return q ? docptr(d)->findLabels(q, resaddresses) : 0; }
size_t RDDocument_FindLabelsR(const RDDocument* d, const char* q, const rd_address** resaddresses) { return q ? docptr(d)->findLabelsR(q, resaddresses) : 0; }

bool RDDocument_GetFunctionGraph(const RDDocument* d, rd_address address, RDGraph** item)
{
    FunctionGraph* g = docptr(d)->getGraph(address);
    if(item && g) *item = CPTR(RDGraph, g);
    return g;
}

void RDDocument_SetSegment(RDDocument* d, const char* name, rd_offset offset, rd_address address, u64 size, rd_flag flags)
{
    if(!name) return;
    docptr(d)->setSegment(name, offset, address, size, size, flags);
}

void RDDocument_SetSegmentRange(RDDocument* d, const char* name, rd_offset offset, rd_address startaddress, rd_address endaddress, rd_flag flags)
{
    if(!name) return;
    size_t range = endaddress - startaddress;
    docptr(d)->setSegment(name, offset, startaddress, range, range, flags);
}

void RDDocument_SetSegmentSize(RDDocument* d, const char* name, rd_offset offset, rd_address address, u64 psize, u64 vsize, rd_flag flags)
{
    if(!name) return;
    docptr(d)->setSegment(name, offset, address, psize, vsize, flags);
}

const char* RDDocument_GetLabel(const RDDocument* d, rd_address address)
{
    static std::string s;

    auto label = docptr(d)->getLabel(address);
    if(!label) return nullptr;

    s = *label;
    return s.c_str();
}

void RDDocument_EachBlock(const RDDocument* d, rd_address address, Callback_Block cb, void* userdata)
{
    if(!cb) return;
    auto* blocks = docptr(d)->getBlocks(address);
    if(blocks) Object::eachT(*blocks, [=](const RDBlock& block) { return cb(&block, userdata); });
}

const char* RDDocument_GetComments(const RDDocument* d, rd_address address)
{
    static std::string s;
    s = docptr(d)->getComments(address);
    return s.c_str();
}
