#pragma once

#include <redasm/pimpl.h>
#include <functional>
#include <libs/miniz/miniz.h>

namespace REDasm {

class AbstractBuffer;

class CompressionImpl
{
    public:
        typedef std::function<int(z_stream*, int)> ZLibFunction;

    public:
        CompressionImpl() = delete;
        static void prepare(z_stream* zs, const AbstractBuffer *buffin, AbstractBuffer* buffout);
        static bool process(z_stream* zs, AbstractBuffer* buffout, const ZLibFunction& func, int funcarg);
};

} // namespace REDasm
