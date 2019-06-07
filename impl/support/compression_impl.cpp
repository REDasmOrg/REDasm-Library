#include "compression_impl.h"
#include <redasm/buffer/abstractbuffer.h>

#define CHUNK_SIZE 16384

namespace REDasm {

void CompressionImpl::prepare(mz_stream *zs, const AbstractBuffer *buffin, AbstractBuffer *buffout)
{
    buffout->resize(CHUNK_SIZE);

    zs->zalloc = Z_NULL;
    zs->zfree = Z_NULL;
    zs->opaque = Z_NULL;

    zs->next_in = reinterpret_cast<Bytef*>(buffin->data());
    zs->avail_in = static_cast<uInt>(buffin->size());

    zs->total_out = 0;
}

bool CompressionImpl::process(mz_stream *zs, AbstractBuffer *buffout, const CompressionImpl::ZLibFunction &func, int funcarg)
{
    int res = 0;

    do
    {
        if(zs->total_out >= static_cast<uLong>(buffout->size()))
            buffout->resize(buffout->size() * 2);

        zs->next_out = reinterpret_cast<Bytef*>(buffout->data() + zs->total_out);
        zs->avail_out = static_cast<uInt>(buffout->size() - zs->total_out);
        res = func(zs, funcarg);

        if(res == Z_STREAM_END)
            break;
    }
    while(res == Z_OK);

    if(buffout->size() > zs->total_out)
        buffout->resize(zs->total_out);

    return res == Z_STREAM_END;
}

} // namespace REDasm
