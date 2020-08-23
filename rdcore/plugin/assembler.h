#pragma once

#include <rdapi/plugin/assembler/assembler.h>
#include <rdapi/types.h>
#include <rdapi/rdil.h>
#include <string>
#include <array>
#include "../buffer/view.h"
#include "../object.h"

class Disassembler;
class EmulateResult;
class ILFunction;

class Assembler: public Object
{
    public:
        Assembler(RDAssemblerPlugin* passembler, Disassembler* disassembler);
        void lift(rd_address address, const RDBufferView* view, ILFunction* il) const;
        bool renderInstruction(RDRenderItemParams* rip);
        bool encode(RDEncodedInstruction* encoded) const;
        void emulate(EmulateResult* result) const;

    public:
        bool getUserData(RDUserData* userdata) const;
        const char* id() const;
        size_t addressWidth() const;
        size_t bits() const;

    private:
        RDAssemblerPlugin* m_passembler;
        Disassembler* m_disassembler;
};

