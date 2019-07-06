#pragma once

#define DEFINE_STATES(...) protected: enum: state_t { __VA_ARGS__ }; private:
#define USER_STATE_START   0x10000000

#include <functional>
#include "../../../disassembler/listing/cachedinstruction.h"
#include "../../../types/api.h"

namespace REDasm {

typedef u32 state_t;

struct State
{
    String name;
    state_t id;

    union {
        u64 u_value;
        s64 s_value;
        address_t address;
    };

    size_t index;
    CachedInstruction instruction;

    bool isFromOperand() const;
    bool isUser() const;
    bool operator ==(const State& rhs) const;
    const Operand* operand() const;
};

typedef std::function<void(const State*)> StateCallback;

} // namespace REDasm
