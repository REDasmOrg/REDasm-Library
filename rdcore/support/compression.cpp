#include "compression.h"
#include <fstream>

#define CHUNK_SIZE 16384

bool Compression::decompress(const RawData& datain, RawData& dataout)
{
    if(datain.empty()) return false;

    mz_stream zs;
    Compression::prepare(&zs, datain, dataout);
    if(mz_inflateInit(&zs) != MZ_OK) return false;

    bool res = Compression::process(&zs, dataout, ::mz_inflate, 0);
    mz_inflateEnd(&zs);
    return res;
}

bool Compression::compressFile(const std::string& filepath, RawData& dataout)
{
    RawData data;
    if(!Compression::readFile(filepath, data)) return false;
    return Compression::compress(data, dataout);
}

bool Compression::decompressFile(const std::string& filepath, RawData& dataout)
{
    RawData data;
    if(!Compression::readFile(filepath, data)) return false;
    return Compression::decompress(data, dataout);
}

bool Compression::readFile(const std::string& filepath, RawData& data)
{
    std::ifstream stream(filepath, std::ios::in | std::ios::binary | std::ios::ate);
    if(!stream.is_open()) return false;

    std::streamsize size = stream.tellg();
    stream.seekg(0, std::ios::beg);

    data.resize(size);
    stream.read(reinterpret_cast<char*>(data.data()), size);
    return true;
}

bool Compression::compress(const RawData& datain, RawData& dataout)
{
    if(datain.empty()) return false;

    mz_stream zs;
    Compression::prepare(&zs, datain, dataout);
    if(mz_deflateInit(&zs, MZ_BEST_COMPRESSION) != MZ_OK) return false;

    bool res = Compression::process(&zs, dataout, ::mz_deflate, MZ_FINISH);
    mz_deflateEnd(&zs);
    return res;
}

bool Compression::process(mz_stream* zs, RawData& dataout, const Compression::ZLibFunction& func, int funcarg)
{
    int res = 0;

    do
    {
        if(zs->total_out >= static_cast<mz_ulong>(dataout.size())) dataout.resize(dataout.size() * 2);

        zs->next_out = reinterpret_cast<unsigned char*>(dataout.data() + zs->total_out);
        zs->avail_out = static_cast<unsigned int>(dataout.size() - zs->total_out);
        res = func(zs, funcarg);

        if(res == MZ_STREAM_END) break;
    }
    while(res == MZ_OK);

    if(dataout.size() > zs->total_out) dataout.resize(zs->total_out);
    return res == MZ_STREAM_END;
}

void Compression::prepare(mz_stream* zs, const RawData& datain, RawData& dataout)
{
    dataout.resize(CHUNK_SIZE);

    zs->zalloc = nullptr;
    zs->zfree = nullptr;
    zs->opaque = nullptr;
    zs->next_in = const_cast<unsigned char*>(datain.data());
    zs->avail_in = static_cast<unsigned int>(datain.size());
    zs->total_out = 0;
}
