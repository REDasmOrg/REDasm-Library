#include "types.h"
#include <rdcore/types/definitions.h>

RDType* RDType_CreateInt(size_t size, bool issigned) { return CPTR(RDType, new IntType(size, issigned)); }
RDType* RDType_CreateFloat(size_t size, bool issigned) { return CPTR(RDType, new FloatType(size, issigned)); }
RDType* RDType_CreateStructure() { return CPTR(RDType, new StructureType()); }
rd_type RDType_GetType(const RDType* type) { return CPTR(const Type, type)->type(); }

bool RDStructure_Append(RDType* s, RDType* t, const char* name)
{
    if(!t || !name) return false;

    auto* st = dynamic_cast<StructureType*>(CPTR(Type, s));
    if(!st) return false;
    st->append(CPTR(Type, t), name);
    return true;
}

