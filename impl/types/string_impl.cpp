#include "string_impl.h"

namespace REDasm {

String StringImpl::xorified() const
{
    std::string s = m_data;
    size_t len = s.size();

    for(size_t i = 0; i < len; i++)
        s[i] ^= ((len - i) % 0x100u);

    return s.c_str();
}

} // namespace REDasm
