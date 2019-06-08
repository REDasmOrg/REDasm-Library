#pragma once

#include <cstddef>

namespace REDasm {

enum class JobState: size_t {
    InactiveState = 0,
    SleepState,
    ActiveState,
    PausedState,
};

} // namespace REDasm
