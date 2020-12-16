#pragma once

#include <rdapi/database/types.h>
#include "definitions.h"

class TypeFactory
{
    public:
        TypeFactory() = delete;
        static StructureType* createStructure();

    private:
        static Type* createType(rd_type type);
};
