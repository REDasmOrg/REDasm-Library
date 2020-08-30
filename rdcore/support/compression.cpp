#include "compression.h"

#define CHUNK_SIZE 16384

bool Compression::decompress(const Compression::Data& datain, Compression::Data& dataout)
{
    if(datain.empty()) return false;

    z_stream zs;
    Compression::prepare(&zs, datain, dataout);
    if(inflateInit(&zs) != Z_OK) return false;

    bool res = Compression::process(&zs, dataout, ::inflate, 0);
    inflateEnd(&zs);
    return res;
}

bool Compression::compress(const Compression::Data& datain, Compression::Data& dataout)
{
    if(datain.empty()) return false;

    z_stream zs;
    Compression::prepare(&zs, datain, dataout);
    if(deflateInit(&zs, Z_BEST_COMPRESSION) != Z_OK) return false;

    bool res = Compression::process(&zs, dataout, ::deflate, Z_FINISH);
    deflateEnd(&zs);
    return res;
}

bool Compression::process(z_stream* zs, Compression::Data& dataout, const Compression::ZLibFunction& func, int funcarg)
{
    int res = 0;

    do
    {
        if(zs->total_out >= static_cast<uLong>(dataout.size())) dataout.resize(dataout.size() * 2);

        zs->next_out = reinterpret_cast<Bytef*>(dataout.data() + zs->total_out);
        zs->avail_out = static_cast<uInt>(dataout.size() - zs->total_out);
        res = func(zs, funcarg);

        if(res == Z_STREAM_END) break;
    }
    while(res == Z_OK);

    if(dataout.size() > zs->total_out) dataout.resize(zs->total_out);
    return res == Z_STREAM_END;
}

void Compression::prepare(z_stream* zs, const Compression::Data& datain, Compression::Data& dataout)
{
    dataout.resize(CHUNK_SIZE);

    zs->zalloc = Z_NULL;
    zs->zfree = Z_NULL;
    zs->opaque = Z_NULL;

    zs->next_in = const_cast<Bytef*>(datain.data());
    zs->avail_in = static_cast<uInt>(datain.size());

    zs->total_out = 0;
}
