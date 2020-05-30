#include "stringfinder.h"
#include "gibberish/gibberishdetector.h"
#include "../document/document.h"
#include "../support/utils.h"
#include "../disassembler.h"
#include "../context.h"
#include <rdapi/symbol.h>
#include <thread>
#include <vector>
#include <cctype>
#include <cuchar>

StringFinder::StringFinder(Disassembler* disassembler, const RDSegment& segment): m_disassembler(disassembler), m_segment(segment) { m_view.reset(m_disassembler->loader()->view(segment)); }

void StringFinder::find()
{
    BufferView view;
    m_view->copyTo(&view);
    bool hasnext = true;

    while(hasnext)
    {
        hasnext = this->step(&view);
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
    if(!c || (c == RD_NPOS) || (c > MB_CUR_MAX)) return false;

    bool res = StringFinder::isAscii(buffer[0]);
    if(res && outch) *outch = buffer[0];
    return res;
}

bool StringFinder::step(BufferView* view)
{
    if(view->empty()) return false;
    RDLocation loc = m_disassembler->loader()->addressof(view->data());
    if(!loc.valid) return false;

    rd_ctx->status("Searching strings @ " + std::string(m_segment.name) + " in " + Utils::hex(loc.value));

    size_t totalsize = 0;
    flag_t flags = this->categorize(view, &totalsize);

    if(flags & SymbolFlags_AsciiString) m_disassembler->document()->asciiString(loc.value, totalsize);
    else if(flags & SymbolFlags_WideString) m_disassembler->document()->wideString(loc.value, totalsize);
    else { view->advance(1); return true; }

    view->advance(totalsize);
    return true;
}

flag_t StringFinder::categorize(const BufferView* view, size_t* totalsize) const
{
    if(view->size() < (sizeof(char) * 2)) return SymbolFlags_None;

    char c1 = static_cast<char>(view->at(0));
    char c2 = static_cast<char>(view->at(1));

    if(StringFinder::isAscii(c1) && !c2)
    {
        std::vector<char> ts;
        BufferView v;
        view->copyTo(&v);
        char16_t wc = *reinterpret_cast<const char16_t*>(v.data());
        char ch = 0;

        for(size_t i = 0; !v.empty() && wc; i++, v.advance(sizeof(char16_t)))
        {
            wc = *reinterpret_cast<const char16_t*>(v.data());

            if(StringFinder::toAscii(wc, &ch))
            {
                ts.push_back(ch);
                continue;
            }

            if(i >= MIN_STRING)
            {
                if(!this->validateString(reinterpret_cast<const char*>(ts.data()), ts.size()))
                    return SymbolFlags_None;

                if(totalsize) *totalsize = i * sizeof(char16_t);
                return SymbolFlags_WideString;
            }

            break;
        }
    }

    for(size_t i = 0; i < view->size(); i++)
    {
        if(StringFinder::isAscii(view->at(i))) continue;

        if(i >= MIN_STRING)
        {
            if(!this->validateString(reinterpret_cast<const char*>(view->data()), i - 1))
                return SymbolFlags_None;

            if(totalsize) *totalsize = i;
            return SymbolFlags_AsciiString;
        }

        break;
    }

    return SymbolFlags_None;
}

bool StringFinder::validateString(const char* s, size_t size) const
{
    if(*s == '%') return true;

    std::string str(s, size);
    if(GibberishDetector::isGibberish(str)) return false;

    double alphacount = static_cast<double>(std::count_if(str.begin(), str.end(), ::isalpha));
    return (alphacount / static_cast<double>(str.size())) > 0.50;
}
