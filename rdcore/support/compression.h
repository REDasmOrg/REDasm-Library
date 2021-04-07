#pragma once

#include <string>
#include <vector>
#include <rdapi/types.h>
#include "../libs/miniz/miniz.h"
#include "../object.h"

class Compression
{
    private:
        typedef int (*ZLibFunction)(mz_stream*, int);

    public:
        Compression() = delete;
        static bool compress(const RawData& datain, RawData& dataout);
        static bool decompress(const RawData& datain, RawData& dataout);
        static bool compressFile(const std::string& filepath, RawData& dataout);
        static bool decompressFile(const std::string& filepath, RawData& dataout);

    private:
        static bool readFile(const std::string& filepath, RawData& data);

    private:
        static bool process(mz_stream* zs, RawData& dataout, const ZLibFunction& func, int funcarg);
        static void prepare(mz_stream* zs, const RawData& datain, RawData& dataout);
};
