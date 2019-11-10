#include "state.h"

namespace REDasm {

bool State::isFromOperand() const { return index != REDasm::npos; }
bool State::isUser() const { return id >= USER_STATE_START; }
bool State::operator ==(const State &rhs) const { return (id == rhs.id) && (address == rhs.address); }
const Operand *State::operand() const { return &instruction->operandsstruct[index]; }

} // namespace REDasm
