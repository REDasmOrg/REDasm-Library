#pragma once

#define MIN_STRING 5

#include <memory>
#include "../buffer/view.h"
#include <rdapi/types.h>

class Disassembler;

class StringFinder
{
    public:
        StringFinder(Disassembler* disassembler, const RDSegment& segment);
        void find();

    public:
        template<typename T> static bool inline isAscii(T c) { return (c >= 0x09 && c <= 0x0D) || (c >= 0x20 && c <= 0x7E); }
        static bool toAscii(char inch, char* outch);
        static bool toAscii(char16_t inch, char* outch);

    private:
        rd_flag categorize(const BufferView* view, size_t* totalsize) const;
        bool validateString(const char* s, size_t size) const;
        bool checkFormats(const std::string& s) const;
        bool step(BufferView* view);

    private:
        Disassembler* m_disassembler;
        std::unique_ptr<BufferView> m_view;
        RDSegment m_segment;
};
