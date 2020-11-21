#include "stringfinder.h"
#include "gibberish/gibberishdetector.h"
#include "../plugin/loader.h"
#include "../document/document.h"
#include "../support/utils.h"
#include "../context.h"
#include "../config.h"
#include <rdapi/symbol.h>
#include <thread>
#include <vector>
#include <cctype>
#include <cuchar>

void StringFinder::find(Context* ctx, const RDBufferView& inview)
{
    RDBufferView view = inview;
    bool hasnext = true;

    while(hasnext)
    {
        hasnext = StringFinder::step(ctx, &view);
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

bool StringFinder::step(Context* ctx, RDBufferView* view)
{
    if(BufferView::empty(view)) return false;
    RDLocation loc = ctx->loader()->addressof(view->data);
    if(!loc.valid) return false;

    rd_cfg->status("Searching strings @ " + Utils::hex(loc.value));

    size_t totalsize = 0;
    rd_flag flags = StringFinder::categorize(view, &totalsize);

    if(StringFinder::checkAndMark(ctx, loc.address, flags, totalsize))
        BufferView::advance(view, totalsize);
    else
        BufferView::advance(view, 1);

    return true;
}

rd_flag StringFinder::categorize(const RDBufferView* view, size_t* totalsize)
{
    if(view->size < (sizeof(char) * 2)) return SymbolFlags_None;

    char c1 = static_cast<char>(view->data[0]);
    char c2 = static_cast<char>(view->data[1]);

    if(StringFinder::isAscii(c1) && !c2)
    {
        std::vector<char> ts;
        RDBufferView v = *view;
        char16_t wc = *reinterpret_cast<const char16_t*>(v.data);
        char ch = 0;

        for(size_t i = 0; !BufferView::empty(&v) && wc; i++, BufferView::advance(&v, sizeof(char16_t)))
        {
            wc = *reinterpret_cast<const char16_t*>(v.data);

            if(StringFinder::toAscii(wc, &ch))
            {
                ts.push_back(ch);
                continue;
            }

            if(i >= MIN_STRING)
            {
                if(!StringFinder::validateString(reinterpret_cast<const char*>(ts.data()), ts.size()))
                    return SymbolFlags_None;

                if(totalsize) *totalsize = i * sizeof(char16_t);
                return SymbolFlags_WideString;
            }

            break;
        }
    }

    for(size_t i = 0; i < view->size; i++)
    {
        if(StringFinder::isAscii(view->data[i])) continue;

        if(i >= MIN_STRING)
        {
            if(!StringFinder::validateString(reinterpret_cast<const char*>(view->data), i - 1))
                return SymbolFlags_None;

            if(totalsize) *totalsize = i;
            return SymbolFlags_AsciiString;
        }

        break;
    }

    return SymbolFlags_None;
}

bool StringFinder::checkAndMark(Context* ctx, rd_address address, rd_flag flags, size_t totalsize)
{
    if(flags & SymbolFlags_AsciiString) return ctx->document()->asciiString(address, totalsize, std::string());
    if(flags & SymbolFlags_WideString) return ctx->document()->wideString(address, totalsize, std::string());
    return false;
}

bool StringFinder::validateString(const char* s, size_t size)
{
    std::string str(s, size);

    switch(str.front()) // Some Heuristics
    {
        case '\'': if((str.back() != '\''))   return false; break;
        case '\"': if((str.back() != '\"'))   return false; break;
        case '<':  if((str.back() != '>'))    return false; break;
        case '(':  if((str.back() != ')'))    return false; break;
        case '[':  if((str.back() != ']'))    return false; break;
        case '{':  if((str.back() != '}'))    return false; break;
        case '%':  if(!StringFinder::checkFormats(str)) return false; break;
        case ' ':  return false; break;
        default:   if(GibberishDetector::isGibberish(str)) return false; break;
    }

    double alphacount = static_cast<double>(std::count_if(str.begin(), str.end(), ::isalpha));
    return (alphacount / static_cast<double>(str.size())) > 0.50;
}

bool StringFinder::checkFormats(const std::string& s)
{
    static std::unordered_set<std::string> formats = {
        "%c", "%d", "%e", "%E", "%f", "%g", "%G",
        "%hi", "%hu", "%i", "%l", "%ld", "%li",
        "%lf", "%Lf", "%lu", "%lli, %lld", "%llu",
        "%o", "%p", "%s", "%u",
        "%x", "%X", "%n", "%%"
    };

    return formats.find(s) != formats.end();
}
