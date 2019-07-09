#pragma once

#define ASSEMBLER_REQUEST(id, mode) { (id), (mode) }

namespace REDasm {

struct AssemblerRequest {
    AssemblerRequest();
    AssemblerRequest(const char* id, const char* mode = nullptr);
    bool modeIs(const char* mode) const;
    bool idIs(const char* mode) const;
    operator const char*() const;

    const char *id, *mode;
};

} // namespace REDasm
