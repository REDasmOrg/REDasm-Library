#pragma once

#include <rdapi/rdil.h>
#include <sstream>
#include <string>

class Disassembler;

class RDIL
{
    public:
        RDIL() = default;
        static const char* mnemonic(rd_instruction_id id);
        static std::string disasm(const Disassembler* disassembler, const RDInstruction* rdil, const RDInstruction* instruction);
        static void emitRDIL(RDInstruction* rdil, rd_instruction_id id);

    private:
        static void darg(std::stringstream& ss, const RDOperand& op, const RDInstruction* instruction, const Disassembler* disassembler, size_t idx);
};

