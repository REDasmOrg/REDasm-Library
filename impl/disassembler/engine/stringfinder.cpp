#include "stringfinder.h"
#include <redasm/disassembler/disassembler.h>
#include <redasm/context.h>
#include <cctype>

namespace REDasm {

StringFinder::StringFinder(const BufferView &view): m_view(view) { }

void StringFinder::find()
{
    auto lock = s_lock_safe_ptr(r_docnew);
    BufferView view = m_view;

    while(!view.eob())
    {
        bool wide = false;
        size_t len = this->locationIsString(view, &wide);

        if(len && ((len >= MIN_STRING) || (*view == '%')))
        {
            address_location loc = r_ldr->addressof(view.data());

            if(loc.valid)
            {
                if(wide) lock->wideString(loc, len);
                else lock->asciiString(loc, len);
            }

            view += len;
        }
        else
            view++;
    }
}

size_t StringFinder::locationIsString(const BufferView& view, bool *wide) const
{
    if(wide)
        *wide = false;

    size_t count = this->locationIsStringT<u8>(view,
                                               [](u16 b) -> bool { return ::isprint(b) || ::isspace(b); },
                                               [](u16 b) -> bool { return (b == '_') || ::isalnum(b) || ::isspace(b); });

    if(count == 1) // Try with wide strings
    {
        count = this->locationIsStringT<u16>(view,
                                             [](u16 wb) -> bool { u8 b1 = wb & 0xFF, b2 = (wb & 0xFF00) >> 8; return !b2 && (::isprint(b1) || ::isspace(b1)); },
                                             [](u16 wb) -> bool { u8 b1 = wb & 0xFF, b2 = (wb & 0xFF00) >> 8; return ( (b1 == '_') || ::isalnum(b1) || ::isspace(b1)) && !b2; });

        if(wide)
            *wide = true;
    }

    return count;
}

StringFinderResult::StringFinderResult() { address = REDasm::invalid_location<address_t>(); }

} // namespace REDasm
