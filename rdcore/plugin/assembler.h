#pragma once

#include <rdapi/plugin/assembler/assembler.h>
#include <rdapi/types.h>
#include <rdapi/rdil.h>
#include <string>
#include <array>
#include "../buffer/view.h"
#include "entry.h"

class Disassembler;
class EmulateResult;
class ILFunction;

class Assembler: public Entry<RDEntryAssembler>
{
    public:
        Assembler(const RDEntryAssembler* entry, Context* ctx);
        void lift(rd_address address, const RDBufferView* view, ILFunction* il) const;
        bool renderInstruction(const RDRendererParams* rp);
        bool renderFunction(const RDRendererParams* rp);
        bool renderSegment(const RDRendererParams* rp);
        bool encode(RDEncodedInstruction* encoded) const;
        void emulate(EmulateResult* result) const;
        size_t addressWidth() const;
        size_t bits() const;
};

