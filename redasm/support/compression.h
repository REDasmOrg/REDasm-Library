#ifndef COMPRESSION_H
#define COMPRESSION_H

#include "../types/buffer/abstractbuffer.h"
#include <functional>
#include <zlib.h>

namespace REDasm {

class Compression
{
    private:
        typedef std::function<int(z_stream*, int)> ZLibFunction;

    public:
        Compression() = delete;
        Compression(const Compression&) = delete;
        Compression& operator =(const Compression&) = delete;

    public:
        static bool deflate(const AbstractBuffer* buffin, AbstractBuffer* buffout);
        static bool inflate(const AbstractBuffer* buffin, AbstractBuffer* buffout);

    private:
        static void prepare(z_stream* zs, const AbstractBuffer *buffin, AbstractBuffer* buffout);
        static bool process(z_stream* zs, AbstractBuffer* buffout, const ZLibFunction& func, int funcarg);
};

} // namespace REDasm

#endif // COMPRESSION_H
