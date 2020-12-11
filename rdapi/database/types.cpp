#include "types.h"
#include <rdcore/types/definitions.h>

RDType* RDType_CreateInt(size_t bits, bool issigned) { return CPTR(RDType, new IntType(bits, issigned)); }
RDType* RDType_CreateFloat(size_t bits, bool issigned) { return CPTR(RDType, new FloatType(bits, issigned)); }
RDType* RDType_CreateStructure() { return CPTR(RDType, new StructureType()); }

bool RDStructure_Append(RDType* s, RDType* t, const char* name)
{
    if(!t || !name) return false;

    auto* st = dynamic_cast<StructureType*>(CPTR(Type, s));
    if(!st) return false;
    st->append(CPTR(Type, t), name);
    return true;
}
