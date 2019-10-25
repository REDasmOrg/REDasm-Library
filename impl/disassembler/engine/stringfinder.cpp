#include "stringfinder.h"
#include <redasm/disassembler/disassembler.h>
#include <redasm/context.h>
#include <cuchar>

namespace REDasm {

StringFinder::StringFinder(const BufferView &view): m_view(view) { }

void StringFinder::find()
{
    auto lock = s_lock_safe_ptr(r_docnew);
    BufferView view = m_view;

    while(!view.eob())
    {
        address_location loc = r_ldr->addressof(view.data());
        if(!loc.valid) break;

        if(loc.value == 0x402001)
        {
            int zzz = 0;
            zzz++;
        }

        size_t totalsize = 0;
        SymbolFlags flags = this->categorize(view, &totalsize);

        if(flags & SymbolFlags::AsciiString) lock->asciiString(loc, totalsize);
        else if(flags & SymbolFlags::WideString) lock->wideString(loc, totalsize);
        else { view++; continue; }

        view += totalsize;
    }
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

SymbolFlags StringFinder::categorize(const BufferView& view, size_t* totalsize)
{
    if(view.size() < (sizeof(char) * 2)) return SymbolFlags::None;

    char c1 = static_cast<char>(view[0]);
    char c2 = static_cast<char>(view[1]);

    if(StringFinder::isAscii(c1) && !c2)
    {
        BufferView v = view;
        char16_t wc = *reinterpret_cast<const char16_t*>(v.data());
        char ch = 0;

        for(size_t i = 0; !v.eob() && wc; i++, v += sizeof(char16_t))
        {
            wc = *reinterpret_cast<const char16_t*>(v.data());
            if(StringFinder::toAscii(wc, &ch)) continue;

            if((i >= MIN_STRING) || (ch == '%'))
            {
                if(totalsize) *totalsize = i * sizeof(char16_t);
                return SymbolFlags::WideString;
            }

            break;
        }
    }

    for(size_t i = 0; i < view.size(); i++)
    {
        if(StringFinder::isAscii(view[i])) continue;

        if((i >= MIN_STRING) || (view[i] == '%'))
        {
            if(totalsize) *totalsize = i;
            return SymbolFlags::AsciiString;
        }

        break;
    }

    return SymbolFlags::None;
}

StringFinderResult::StringFinderResult() { address = REDasm::invalid_location<address_t>(); }

} // namespace REDasm
