#pragma once

#define MIN_STRING          4

#include <functional>
#include <redasm/plugins/loader/loader.h>
#include <redasm/buffer/bufferview.h>

namespace REDasm {

struct StringFinderResult
{
    StringFinderResult();

    address_location address;
    size_t len{REDasm::npos};
    bool wide{false};
};

class StringFinder
{
    public:
        StringFinder(const BufferView& view);
        void find();

    public:
        template<typename T> static bool inline isAscii(T c) { return (c >= 0x09 && c <= 0x0D) || (c >= 0x20 && c <= 0x7E); }
        static inline bool toAscii(char inch, char* outch) { if(!isAscii(inch)) return false; *outch = inch; return true; }
        static bool toAscii(char16_t inch, char* outch);

    private:
        SymbolFlags categorize(const BufferView& view, size_t* totalsize);
        bool validateString(const char* s, size_t size);
        bool step(BufferView& view);
        void findAsync();
        void findSync();

    private:
        BufferView m_view;
};

} // namespace REDasm
