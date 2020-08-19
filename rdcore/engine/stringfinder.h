#pragma once

#define MIN_STRING 4

#include <memory>
#include "../buffer/view.h"
#include <rdapi/types.h>

class Disassembler;

class StringFinder
{
    public:
        StringFinder() = delete;
        static rd_flag categorize(const RDBufferView* view, size_t* totalsize);
        static bool checkAndMark(Disassembler* disassembler, rd_address address, rd_flag flags, size_t totalsize);
        static void find(Disassembler* disassembler, const RDBufferView& inview);

    public:
        template<typename T> static bool inline isAscii(T c) { return (c >= 0x09 && c <= 0x0D) || (c >= 0x20 && c <= 0x7E); }
        static bool toAscii(char inch, char* outch);
        static bool toAscii(char16_t inch, char* outch);

    private:
        static bool step(Disassembler* disassembler, RDBufferView* view);
        static bool validateString(const char* s, size_t size);
        static bool checkFormats(const std::string& s);
};
