#include "utils.h"
#include <cctype>

namespace REDasm {

bool Utils::byte(const String& s, u8* val, size_t offset)
{
    if(offset > (s.size() - 2))
        return false;

    if(!std::isxdigit(s[offset]) || !std::isxdigit(s[offset + 1]))
        return false;

    *val = static_cast<u8>(s.substring(offset, 2).toInt(16));
    return true;
}

} // namespace REDasm
