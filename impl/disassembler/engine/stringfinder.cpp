#include "stringfinder.h"
#include "gibberish/gibberishdetector.h"
#include <redasm/disassembler/disassembler.h>
#include <redasm/support/jobmanager.h>
#include <redasm/support/utils.h>
#include <redasm/context.h>
#include <cctype>
#include <cuchar>

namespace REDasm {

StringFinder::StringFinder(const BufferView &view): m_view(view) { }

void StringFinder::find()
{
    if(r_ctx->sync()) this->findSync();
    else this->findAsync();
}

bool StringFinder::toAscii(char16_t inch, char* outch)
{
    static std::vector<char> buffer(MB_CUR_MAX);
    size_t c = std::c16rtomb(buffer.data(), inch, nullptr);
    if(!c || (c == REDasm::npos) || (c > MB_CUR_MAX)) return false;

    bool res = StringFinder::isAscii(buffer[0]);
    if(res) *outch = buffer[0];
    return res;
}

void StringFinder::findSync()
{
    BufferView view = m_view;
    bool done = false;

    while(!done)
        done = this->step(view);
}

void StringFinder::findAsync()
{
    BufferView view = m_view;
    Utils::yloop(std::bind(&StringFinder::step, this, view));
}

bool StringFinder::step(BufferView& view)
{
    if((!r_ctx->sync() && !JobManager::initialized()) || view.eob()) return false;
    address_location loc = r_ldr->addressof(view.data());
    if(!loc.valid) return false;

    r_ctx->status("Searching strings @ " + String::hex(loc.value));

    size_t totalsize = 0;
    SymbolFlags flags = this->categorize(view, &totalsize);

    if(flags & SymbolFlags::AsciiString) r_docnew->asciiString(loc, totalsize);
    else if(flags & SymbolFlags::WideString) r_docnew->wideString(loc, totalsize);
    else { view++; return true; }

    view += totalsize;
    return true;
}

SymbolFlags StringFinder::categorize(const BufferView& view, size_t* totalsize)
{
    if(view.size() < (sizeof(char) * 2)) return SymbolFlags::None;

    char c1 = static_cast<char>(view[0]);
    char c2 = static_cast<char>(view[1]);

    if(StringFinder::isAscii(c1) && !c2)
    {
        std::vector<char> ts;
        BufferView v = view;
        char16_t wc = *reinterpret_cast<const char16_t*>(v.data());
        char ch = 0;

        for(size_t i = 0; !v.eob() && wc; i++, v += sizeof(char16_t))
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
                    return SymbolFlags::None;

                if(totalsize) *totalsize = i * sizeof(char16_t);
                return SymbolFlags::WideString;
            }

            break;
        }
    }

    for(size_t i = 0; i < view.size(); i++)
    {
        if(StringFinder::isAscii(view[i])) continue;

        if(i >= MIN_STRING)
        {
            if(!this->validateString(reinterpret_cast<const char*>(view.data()), i - 1))
                return SymbolFlags::None;

            if(totalsize) *totalsize = i;
            return SymbolFlags::AsciiString;
        }

        break;
    }

    return SymbolFlags::None;
}

bool StringFinder::validateString(const char* s, size_t size)
{
    if(*s == '%') return true;

    std::string str(s, size);

    if(GibberishDetector::isGibberish(str)) return false;

    double alphacount = static_cast<double>(std::count_if(str.begin(), str.end(), ::isalpha));
    return (alphacount / static_cast<double>(str.size())) > 0.50;
}

StringFinderResult::StringFinderResult() { address = REDasm::invalid_location<address_t>(); }

} // namespace REDasm