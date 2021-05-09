#include "stringfinder.h"
#include "gibberish/gibberishdetector.h"
#include "../plugin/loader.h"
#include "../document/document.h"
#include "../support/utils.h"
#include "../context.h"
#include "../config.h"
#include <unordered_set>
#include <thread>
#include <cctype>
#include <cuchar>

#define ALPHA_THRESHOLD 0.5

std::string StringFinder::m_tempstr;

void StringFinder::find(Context* ctx, const RDBufferView& inview)
{
    RDBufferView view = inview;
    bool hasnext = true;

    while(hasnext)
    {
        hasnext = StringFinder::step(ctx, view);
        std::this_thread::yield();
    }
}

bool StringFinder::toAscii(char inch, char* outch)
{
    if(!StringFinder::isAscii(inch)) return false;
    if(outch) *outch = inch;
    return true;
}

bool StringFinder::toAscii(char16_t inch, char* outch)
{
    static std::vector<char> buffer(MB_CUR_MAX);
    size_t c = std::c16rtomb(buffer.data(), inch, nullptr);
    if(!c || (c == RD_NVAL) || (c > MB_CUR_MAX)) return false;

    bool res = StringFinder::isAscii(buffer[0]);
    if(res && outch) *outch = buffer[0];
    return res;
}

bool StringFinder::step(Context* ctx, RDBufferView& view)
{
    if(BufferView::empty(&view)) return false;
    RDLocation loc = ctx->document()->addressof(view.data);
    if(!loc.valid) return false;

    rd_cfg->status("Searching strings @ " + Utils::hex(loc.value));

    size_t totalsize = 0;
    rd_flag flags = StringFinder::categorize(ctx, view, &totalsize);

    if(StringFinder::checkAndMark(ctx, loc.address, flags, totalsize))
        BufferView::advance(&view, totalsize);
    else
        BufferView::advance(&view, 1);

    return true;
}

rd_flag StringFinder::categorize(Context* ctx, const RDBufferView& view, size_t* totalsize)
{
    if(view.size < (sizeof(char) * 2)) return AddressFlags_None;

    char c1 = static_cast<char>(view.data[0]);
    char c2 = static_cast<char>(view.data[1]);
    bool ok = false;

    if(StringFinder::isAscii(c1) && !c2)
    {
        ok = StringFinder::categorizeT<char16_t>(view, ctx->minString(), totalsize, [](char16_t ch, char* outch) {
            return StringFinder::toAscii(ch, outch);
        });

        if(ok) return AddressFlags_WideString;
    }

    ok = StringFinder::categorizeT<char>(view, ctx->minString(), totalsize, [](char ch, char* outch) {
        if(!StringFinder::isAscii(ch)) return false;
        *outch = ch;
        return true;
    });

    return ok ? AddressFlags_AsciiString : AddressFlags_None;
}

bool StringFinder::checkAndMark(Context* ctx, rd_address address, rd_flag flags, size_t totalsize)
{
    if((flags & AddressFlags_AsciiString) || (flags & AddressFlags_WideString))
        return ctx->document()->setString(address, totalsize, flags);

    return false;
}

bool StringFinder::validateString(const std::string& str)
{
    if(!StringFinder::checkHeuristic(str, true)) return false;

    double alnumcount = static_cast<double>(std::count_if(str.begin(), str.end(), ::isalnum));
    return (alnumcount / static_cast<double>(str.size())) > ALPHA_THRESHOLD;
}

bool StringFinder::checkHeuristic(const std::string& s, bool gibberish)
{
    switch(s.front())
    {
        case '\'':
            if((s.back() != '\'')) break;
            [[fallthrough]];

        case '\"':
            if((s.back() != '\"')) break;
            [[fallthrough]];

        case '<':
            if((s.back() != '>'))  break;
            [[fallthrough]];

        case '(':
            if((s.back() != ')'))  break;
            [[fallthrough]];

        case '[':
            if((s.back() != ']'))  break;
            [[fallthrough]];

        case '{':
            if((s.back() != '}'))  break;
            [[fallthrough]];

        case ' ': break;
        case '%': return StringFinder::checkFormats(s);
        default:  return gibberish && !GibberishDetector::isGibberish(s);
    }

    return false;
}

bool StringFinder::checkFormats(const std::string& s)
{
    static const std::unordered_set<std::string> FORMATS = {
        "%c", "%d", "%e", "%E", "%f", "%g", "%G",
        "%hi", "%hu", "%i", "%l", "%ld", "%li",
        "%lf", "%Lf", "%lu", "%lli", "%lld", "%llu",
        "%o", "%p", "%s", "%u",
        "%x", "%X", "%n", "%%"
    };

    return FORMATS.find(s) != FORMATS.end();
}
