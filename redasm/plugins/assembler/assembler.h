#pragma once

#include "../../buffer/bufferview.h"
#include "../../types/api_types.h"
#include "../plugin.h"
#include "algorithm/algorithm.h"
#include "printer/printer.h"

namespace REDasm {

class ListingDocumentIterator;
class AssemblerImpl;
class Disassembler;
struct Symbol;

class LIBREDASM_API Assembler : public Plugin
{
    PIMPL_DECLARE_PRIVATE(Assembler)

    protected:
        Assembler(AssemblerImpl* p);

    public:
        Assembler();
        object_ptr<Algorithm> createAlgorithm(Disassembler* disassembler) { return REDasm::wrap_object<Algorithm>(this->doCreateAlgorithm(disassembler)); }
        object_ptr<Printer> createPrinter(Disassembler* disassembler) { return REDasm::wrap_object<Printer>(this->doCreatePrinter(disassembler)); }
        size_t addressWidth() const;
        virtual size_t bits() const = 0;
        virtual bool decode(const BufferView &view, const InstructionPtr& instruction);
        virtual bool decodeInstruction(const BufferView& view, const InstructionPtr& instruction);
        virtual Symbol* findTrampoline(ListingDocumentIterator* it) const;

    protected:
        void setInstructionType(instruction_id_t id, InstructionType type);
        virtual Algorithm* doCreateAlgorithm(Disassembler* disassembler) const;
        virtual Printer* doCreatePrinter(Disassembler* disassembler) const;
        virtual void onDecoded(const InstructionPtr& instruction);
};

} // namespace REDasm
