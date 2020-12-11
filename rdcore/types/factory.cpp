#include "factory.h"
#include "../support/error.h"

IntType* TypeFactory::createInt(size_t bsize, bool issigned)
{
}

FloatType* TypeFactory::createFloat(size_t bsize, bool issigned)
{

}

StructureType* TypeFactory::createStructure() { return new StructureType(); }

Type* TypeFactory::createType(rd_type type)
{
}
