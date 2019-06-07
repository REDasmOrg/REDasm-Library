#pragma once

#define DEFINE_STATES(...) protected: enum: state_t { __VA_ARGS__ }; private:
#define USER_STATE_START   0x10000000

#include <redasm/types/api_types.h>

namespace REDasm {

typedef u32 state_t;

struct State
{
    std::string name;
    state_t id;

    union {
        u64 u_value;
        s64 s_value;
        address_t address;
    };

    size_t index;
    InstructionPtr instruction;

    bool isFromOperand() const;
    bool isUser() const;
    bool operator ==(const State& rhs) const;
    const Operand* operand() const;
};

} // namespace REDasm
