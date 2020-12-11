#pragma once

#include <rdapi/database/types.h>
#include "definitions.h"

class TypeFactory
{
    public:
        TypeFactory() = delete;
        static IntType* createInt(size_t bsize, bool issigned);
        static FloatType* createFloat(size_t bsize, bool issigned);
        static StructureType* createStructure();

    private:
        static Type* createType(rd_type type);
};
