#pragma once

#include <vector>
#include <rdapi/types.h>
#include "../libs/miniz/miniz.h"

#undef compress

class Compression
{
    private:
        typedef int (*ZLibFunction)(z_stream*, int);

    public:
        typedef std::vector<u8> Data;

    public:
        Compression() = delete;
        static bool compress(const Data& datain, Data& dataout);
        static bool decompress(const Data& datain, Data& dataout);

    private:
        static bool process(z_stream* zs, Data& dataout, const ZLibFunction& func, int funcarg);
        static void prepare(z_stream* zs, const Data& datain, Data& dataout);
};
