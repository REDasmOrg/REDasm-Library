#pragma once

#include "../../buffer/bufferview.h"
#include "../../types/api_types.h"
#include "../plugin.h"
#include "algorithm/algorithm.h"
#include "printer/printer.h"
#include "assemblerrequest.h"

#define REGISTER_INSTRUCTION(id, cb)    this->registerInstruction(id, std::bind(cb, this, std::placeholders::_1))
#define SET_INSTRUCTION_TYPE(id, type)  this->setInstructionType(id, type)

namespace REDasm {

class ListingDocumentIterator;
class AssemblerImpl;
class Disassembler;
struct Symbol;

class LIBREDASM_API Assembler : public Plugin
{
    PIMPL_DECLARE_PRIVATE(Assembler)

    protected:
        typedef std::function<void(const InstructionPtr&)> InstructionCallback;

    protected:
        Assembler(AssemblerImpl* p);

    public:
        Assembler();
        object_ptr<Algorithm> createAlgorithm(Disassembler* disassembler) { return REDasm::wrap_object<Algorithm>(this->doCreateAlgorithm(disassembler)); }
        object_ptr<Printer> createPrinter(Disassembler* disassembler) { return REDasm::wrap_object<Printer>(this->doCreatePrinter(disassembler)); }
        const AssemblerRequest &request() const;
        size_t addressWidth() const;
        virtual size_t bits() const = 0;
        virtual void init(const AssemblerRequest &request);
        virtual bool decode(const BufferView &view, const InstructionPtr& instruction);
        virtual bool decodeInstruction(const BufferView& view, const InstructionPtr& instruction);
        virtual Symbol* findTrampoline(ListingDocumentIterator* it) const;

    protected:
        void setInstructionType(instruction_id_t id, InstructionType type);
        void registerInstruction(instruction_id_t id, const InstructionCallback &cb);
        virtual Algorithm* doCreateAlgorithm(Disassembler* disassembler) const;
        virtual Printer* doCreatePrinter(Disassembler* disassembler) const;
        virtual void onDecoded(const InstructionPtr& instruction);
};

} // namespace REDasm
