#pragma once

#define ASSEMBLER_REQUEST(id, mode) { (id), (mode) }

#include "../../types/string.h"

namespace REDasm {

struct AssemblerRequest {
    AssemblerRequest();
    AssemblerRequest(const char* id, const char* mode = nullptr);
    bool modeIs(const String& mode) const;
    bool idIs(const String& id) const;
    operator String() const;

    String id, mode;
};

} // namespace REDasm
