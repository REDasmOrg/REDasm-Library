#pragma once

#include "../../buffer/bufferview.h"
#include "../../types/api.h"
#include "../plugin.h"
#include "algorithm/algorithm.h"
#include "printer/printer.h"
#include "assemblerrequest.h"

#define REGISTER_INSTRUCTION(id, cb)    this->registerInstruction(id, std::bind(cb, this, std::placeholders::_1))
#define SET_INSTRUCTION_TYPE(id, type)  this->setInstructionType(id, type)

namespace REDasm {

class AssemblerImpl;
class Disassembler;
struct Symbol;

class LIBREDASM_API Assembler : public Plugin
{
    PIMPL_DECLARE_PRIVATE(Assembler)

    protected:
        typedef std::function<void(Instruction*)> InstructionCallback;

    protected:
        Assembler(AssemblerImpl* p);

    public:
        Assembler();
        safe_ptr<Algorithm> createAlgorithm() { return REDasm::wrap_safe_object<Algorithm>(this->doCreateAlgorithm()); }
        object_ptr<Printer> createPrinter() { return REDasm::wrap_object<Printer>(this->doCreatePrinter()); }
        const AssemblerRequest &request() const;
        size_t addressWidth() const;
        virtual size_t bits() const = 0;
        virtual void init(const AssemblerRequest &request);
        virtual bool decode(const BufferView &view, Instruction* instruction);
        virtual bool decodeInstruction(const BufferView& view, Instruction* instruction);
        virtual const Symbol* findTrampoline(size_t index) const;

    protected:
        void setInstructionType(instruction_id_t id, InstructionType type);
        void registerInstruction(instruction_id_t id, const InstructionCallback &cb);
        virtual Algorithm* doCreateAlgorithm() const;
        virtual Printer* doCreatePrinter() const;
        virtual void onDecoded(Instruction* instruction);
};

} // namespace REDasm
