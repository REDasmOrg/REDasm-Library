#include "utils.h"
#include <algorithm>
#include <cctype>
#include <thread>
#include <chrono>
#include <impl/support/utils_impl.h>
#include <impl/disassembler/engine/gibberish/gibberishdetector.h>

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

bool Utils::isGibberishString(const String& s) { return GibberishDetector::isGibberish(s.c_str()); }

void Utils::yloop(const std::function<bool()>& cb)
{
    while(cb())
        std::this_thread::yield();
}

void Utils::sloop(const std::function<bool ()>& cb)
{
    static const std::chrono::milliseconds JOB_BASE_INTERVAL(1); // 1ms
    static const std::chrono::milliseconds JOB_MAX_INTERVAL(5); // 5ms
    auto interval = JOB_BASE_INTERVAL;
    auto start = std::chrono::steady_clock::now();

    while(cb())
    {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
        interval = std::min(std::max((interval + elapsed) / 2, JOB_BASE_INTERVAL), JOB_MAX_INTERVAL);

        std::this_thread::sleep_for(interval);
        start = std::chrono::steady_clock::now();
    }
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
