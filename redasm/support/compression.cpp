#include "compression.h"
#include "../buffer/abstractbuffer.h"
#include <impl/support/compression_impl.h>

#undef deflate
#undef inflate

namespace REDasm {

bool Compression::deflate(const AbstractBuffer *buffin, AbstractBuffer *buffout)
{
    if(buffin->empty())
        return false;

    z_stream zs;
    CompressionImpl::prepare(&zs, buffin, buffout);

    if(deflateInit(&zs, Z_BEST_COMPRESSION) != Z_OK)
        return false;

    bool res = CompressionImpl::process(&zs, buffout, ::mz_deflate, Z_FINISH);
    deflateEnd(&zs);
    return res;
}

bool Compression::inflate(const AbstractBuffer *buffin, AbstractBuffer *buffout)
{
    if(buffin->empty())
        return false;

    z_stream zs;
    CompressionImpl::prepare(&zs, buffin, buffout);

    if(inflateInit(&zs) != Z_OK)
        return false;

    bool res = CompressionImpl::process(&zs, buffout, ::mz_inflate, 0);
    inflateEnd(&zs);
    return res;
}


} // namespace REDasm
