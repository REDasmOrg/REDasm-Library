#pragma once

#include "../macros.h"

namespace REDasm {

class AbstractBuffer;

class LIBREDASM_API Compression
{
    public:
        Compression() = delete;
        static bool deflate(const AbstractBuffer* buffin, AbstractBuffer* buffout);
        static bool inflate(const AbstractBuffer* buffin, AbstractBuffer* buffout);
};

} // namespace REDasm
