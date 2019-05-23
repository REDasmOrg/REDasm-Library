#pragma once

#include "../disassembler/disassemblerapi.h"
#include "../types/buffer/memorybuffer.h"
#include "../support/dispatcher.h"

#define EMULATE_INSTRUCTION(id, callback) m_dispatcher[id] = std::bind(callback, this, std::placeholders::_1)
#define STACK_SIZE                        0xFFFF

namespace REDasm {

class Emulator
{
    private:
        typedef Dispatcher<instruction_id_t, const InstructionPtr&> DispatcherType;
        typedef std::unordered_map< const Segment*, std::unique_ptr<MemoryBuffer> > MappedMemory;

    public:
        Emulator(DisassemblerAPI* disassembler);
        virtual ~Emulator() = default;
        virtual void emulate(const InstructionPtr& instruction);
        virtual bool hasError() const = 0;
        virtual bool read(const Operand* op, u64* value) = 0;
        virtual bool displacement(const Operand* op, u64* value) = 0;

    protected:
        virtual bool setTarget(const InstructionPtr& instruction);
        MemoryBuffer* getSegmentMemory(address_t address, offset_t* offset);
        BufferView getMemory(address_t address);
        BufferView getStack(offset_t sp);

    private:
        void remap();

    protected:
        InstructionPtr m_currentinstruction;
        DisassemblerAPI* m_disassembler;
        DispatcherType m_dispatcher;
        MappedMemory m_memory;
        std::unique_ptr<MemoryBuffer> m_stack;
};

} // namespace REDasm
