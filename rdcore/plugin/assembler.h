#pragma once

#include <rdapi/plugin/assembler.h>
#include <rdapi/types.h>
#include <rdapi/rdil.h>
#include <string>
#include <array>
#include "../buffer/view.h"
#include "../object.h"

class Disassembler;

class Assembler: public Object
{
    public:
        Assembler(RDAssemblerPlugin* passembler, Disassembler* disassembler);
        RDInstruction* emitRDIL(const RDInstruction* instruction, size_t* len);
        std::string registerName(const RDInstruction* instruction, const RDOperand* op, rd_register_id r) const;
        bool isStop(const RDInstruction* instruction) const;
        bool decode(BufferView* view, RDInstruction* instruction) const;
        bool encode(RDEncodedInstruction* encoded) const;
        bool render(RDRenderItemParams* rip) const;
        void emulate(const RDInstruction* instruction);
        void rdil(const RDInstruction* instruction);

    public:
        const char* id() const;
        size_t addressWidth() const;
        size_t bits() const;

    private:
        std::array<RDInstruction, RDIL_INSTRUCTION_COUNT> m_rdilres;
        RDAssemblerPlugin* m_passembler;
        Disassembler* m_disassembler;
};

