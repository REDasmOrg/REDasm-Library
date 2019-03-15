#ifndef AVR8_DECODER_H
#define AVR8_DECODER_H

#include <forward_list>
#include "../../redasm.h"

namespace REDasm {

struct AVR8Operand { u16 mask; u64 tag; };

struct AVR8Instruction
{
    u16 mask;
    instruction_id_t id;
    std::string mnemonic;
    u64 size;
    std::list<AVR8Operand> operands;
};

class AVR8Decoder
{
    public:
        AVR8Decoder() = delete;
        static const AVR8Instruction *get(u16 opcode);

    private:
        static void initializeInstructions();

    private:
        static std::forward_list<AVR8Instruction> m_avrinstructions;
};

} // namespace REDasm

#endif // AVR8_DECODER_H
