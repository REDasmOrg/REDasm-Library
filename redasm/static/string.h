#pragma once

#include <cstddef>

namespace REDasm {

constexpr size_t static_strlen(const char* s) { return *s ? 1 + static_strlen(s + 1) : 0; }

} // namespace REDasm
