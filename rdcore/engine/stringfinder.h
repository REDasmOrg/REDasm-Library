#pragma once

#include <memory>
#include <string>
#include <rdapi/types.h>
#include "../buffer/view.h"

class Context;

class StringFinder
{
    public:
        StringFinder() = delete;
        static rd_flag categorize(Context* ctx, const RDBufferView& view, size_t* totalsize);
        static bool checkAndMark(Context* ctx, rd_address address, rd_flag flags, size_t totalsize);
        static void find(Context* ctx, const RDBufferView& inview);

    public:
        template<typename T> static bool inline isAscii(T c) { return (c >= 0x09 && c <= 0x0D) || (c >= 0x20 && c <= 0x7E); }
        static bool toAscii(char inch, char* outch);
        static bool toAscii(char16_t inch, char* outch);

    private:
        template<typename T, typename ToAsciiCallback> static bool categorizeT(RDBufferView view, size_t minstring, size_t* totalsize, const ToAsciiCallback& cb);
        static bool step(Context* ctx, RDBufferView& view);
        static bool validateString(const std::string& str);
        static bool checkHeuristic(const std::string& s, bool gibberish);
        static bool checkFormats(const std::string& s);

    private:
        static std::string m_tempstr;
};

template<typename T, typename ToAsciiCallback>
bool StringFinder::categorizeT(RDBufferView view, size_t minstring, size_t* totalsize, const ToAsciiCallback& cb) {
    m_tempstr.clear();
    char ch;

    for( ; !BufferView::empty(&view); BufferView::advance(&view, sizeof(T))) {
        if(!cb(*reinterpret_cast<const T*>(view.data), &ch)) break;
        m_tempstr.push_back(ch);
    }

    if(totalsize) {
        *totalsize = m_tempstr.size() * sizeof(T);
        if(!(*view.data)) *totalsize += sizeof(T); // Include null terminator too
    }

    if(m_tempstr.size() >= minstring) return StringFinder::validateString(m_tempstr);
    return StringFinder::checkHeuristic(m_tempstr, false);
}
