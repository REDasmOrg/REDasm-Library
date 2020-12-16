#include "factory.h"
#include "../support/error.h"

StructureType* TypeFactory::createStructure() { return new StructureType(); }

Type* TypeFactory::createType(rd_type type)
{
}
