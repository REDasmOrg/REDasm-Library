#pragma once

#include "../../buffer/bufferview.h"
#include "../../types/api_types.h"
#include "../plugin.h"

namespace REDasm {

class ListingDocumentIterator;
class AssemblerImpl;
class Disassembler;
class Algorithm;
struct Symbol;

class LIBREDASM_API Assembler : public Plugin
{
    PIMPL_DECLARE_PRIVATE(Assembler)

    protected:
        Assembler(AssemblerImpl* p);

    public:
        Assembler();
        Algorithm* algorithm(Disassembler* disassembler);
        virtual size_t bits() const = 0;
        virtual bool decode(const BufferView &view, const InstructionPtr& instruction);
        virtual bool decodeInstruction(const BufferView& view, const InstructionPtr& instruction);
        virtual Symbol* findTrampoline(ListingDocumentIterator* it) const;

    public:
        size_t addressWidth() const;

    protected:
        void setInstructionType(instruction_id_t id, InstructionType type);
        virtual Algorithm* createAlgorithm(Disassembler* disassembler) const;
        virtual void onDecoded(const InstructionPtr& instruction);
};

} // namespace REDasm
