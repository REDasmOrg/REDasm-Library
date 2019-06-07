#pragma once

#include <cstdint>
#include "macros.h"

#define REDASM_API_MAJOR             1
#define REDASM_API_MINOR             0
#define R_VERSION(major, minor) static_cast<REDasm::version_t>(((major) << 8) | ((minor) << 0))
#define REDASM_API_VERSION           R_VERSION(REDASM_API_MAJOR, REDASM_API_MINOR)
#define REDASM_API_VERSION_STRING    STRINGIFY(REDASM_API_MAJOR) "." STRINGIFY(REDASM_API_MINOR)

namespace REDasm {

typedef uint32_t version_t;

inline version_t versionMajor(version_t version) { return (version >> 8) & 0xFF; }
inline version_t versionMinor(version_t version) { return (version >> 0)  & 0xFF; }

} // namespace REDasm

