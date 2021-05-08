#include "types.h"
#include <rdcore/types/definitions.h>

RDType* RDType_CreateInt(size_t size, bool issigned) { return CPTR(RDType, new IntType(size, issigned)); }
RDType* RDType_CreateFloat(size_t size, bool issigned) { return CPTR(RDType, new FloatType(size, issigned)); }
RDType* RDType_CreateArray(size_t itemscount, RDType* itemtype) { return CPTR(RDType, new ArrayType(itemscount, CPTR(Type, itemtype))); }
RDType* RDType_CreateAsciiString(size_t size) { return CPTR(RDType, new AsciiStringType(size)); }
RDType* RDType_CreateWideString(size_t size) { return CPTR(RDType, new WideStringType(size)); }
RDType* RDType_CreateStructure(const char* name) { return CPTR(RDType, new StructureType(name ? name : std::string())); }
RDType* RDType_Clone(const RDType* type) { return CPTR(RDType, CPTR(const Type, type)->clone()); }
rd_type RDType_GetType(const RDType* type) { return CPTR(const Type, type)->type(); }
size_t RDType_GetSize(const RDType* type) { return CPTR(const Type, type)->size(); }

bool RDStructure_Append(RDType* s, RDType* t, const char* name)
{
    if(!t || !name) return false;

    auto* st = dynamic_cast<StructureType*>(CPTR(Type, s));
    if(!st) return false;
    st->append(CPTR(Type, t), name);
    return true;
}

void RDStructure_GetFields(const RDType* s, Callback_StructureFields cb, void* userdata)
{
    if(!s || !cb) return;

    auto* st = dynamic_cast<const StructureType*>(CPTR(const Type, s));
    if(!st) return;

    for(const auto& [n, f] : st->fields())
    {
        if(!cb(n.c_str(), CPTR(const RDType, f), userdata)) break;
    }
}

const char* RDType_GetTypeName(const RDType* type)
{
    static std::string tn;

    if(!type) return nullptr;
    tn = CPTR(const Type, type)->typeName();
    return tn.c_str();
}

const char* RDType_GetName(const RDType* type) { return CPTR(const Type, type)->name().c_str(); }
void RDType_SetName(RDType* type, const char* n) { CPTR(Type, type)->setName(n); }

size_t RDArray_GetItemsCount(const RDType* s)
{
    auto* at = dynamic_cast<const ArrayType*>(CPTR(const Type, s));
    return at ? at->itemsCount() : 0;
}

const RDType* RDArray_GetType(const RDType* s)
{
    auto* at = dynamic_cast<const ArrayType*>(CPTR(const Type, s));
    return CPTR(const RDType, at->type());
}
