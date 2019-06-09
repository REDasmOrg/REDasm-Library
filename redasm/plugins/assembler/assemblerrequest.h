#pragma once

namespace REDasm {

struct AssemblerRequest {
    AssemblerRequest();
    AssemblerRequest(const char* id);
    bool modeIs(const char* mode) const;
    bool idIs(const char* mode) const;
    operator const char*() const;

    const char *id, *mode;
};

} // namespace REDasm
