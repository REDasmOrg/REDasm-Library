#include "utils.h"
#include <cctype>
#include <impl/support/utils_impl.h>

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

template<typename T> T Utils::bitreverse(T val) { return UtilsImpl::bitreverse(val); }

template s8 Utils::bitreverse<s8>(s8);
template s16 Utils::bitreverse<s16>(s16);
template s32 Utils::bitreverse<s32>(s32);
template s64 Utils::bitreverse<s64>(s64);
template u8 Utils::bitreverse<u8>(u8);
template u16 Utils::bitreverse<u16>(u16);
template u32 Utils::bitreverse<u32>(u32);
template u64 Utils::bitreverse<u64>(u64);

} // namespace REDasm
