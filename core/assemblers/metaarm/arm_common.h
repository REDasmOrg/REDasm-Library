#pragma once

#define ARM_REGISTER(reg) ((reg == ARM_REG_INVALID) ? REGISTER_INVALID : reg)

#include <capstone.h>
#include "../../plugins/assembler/assembler.h"

namespace REDasm {

class ARMAbstractAssembler
{
    public:
        virtual ~ARMAbstractAssembler() { }
        virtual u64 pc(const InstructionPtr& instruction) const = 0;
};

template<cs_arch arch, size_t mode> class ARMCommonAssembler: public CapstoneAssemblerPlugin<arch, mode>, public ARMAbstractAssembler
{
    public:
        ARMCommonAssembler();
        virtual ~ARMCommonAssembler();
        bool isPC(const Operand* op) const { return op && (op->is(OperandType::Register) && this->isPC(op->reg.r)); };
        bool isLR(const Operand* op) const { return op && (op->is(OperandType::Register) && this->isLR(op->reg.r)); };

    protected:
        void onDecoded(const InstructionPtr& instruction) override;

    private:
        bool isPC(register_id_t reg) const { return reg == ARM_REG_PC; };
        bool isLR(register_id_t reg) const { return reg == ARM_REG_LR; };
        void checkB(const InstructionPtr& instruction) const;
        void checkStop(const InstructionPtr& instruction) const;
        void checkStop_0(const InstructionPtr& instruction) const;
        void checkJumpT0(const InstructionPtr& instruction) const;
        void checkCallT0(const InstructionPtr& instruction) const;
};

} // namespace REDasm

#include "arm_common.hpp"
